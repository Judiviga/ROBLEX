#include "ROBLEX.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>

// UUIDs del Nordic UART Service (NUS). DEBEN coincidir con la app de control.
#define ROBLEX_BLE_SERVICE "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define ROBLEX_BLE_RX      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // app -> robot
#define ROBLEX_BLE_TX      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // robot -> app

// Estado interno del BLE (un unico servidor por robot).
static BLECharacteristic *_roblexTx = nullptr;
static volatile bool _roblexConnected = false;
static String _roblexRxBuffer = "";   // arma la linea actual
static String _roblexRxLine = "";     // ultima linea completa recibida
static volatile bool _roblexLineReady = false;

// BLEDevice::init() solo debe llamarse una vez.
static bool _roblexBleInited = false;
static void _roblexEnsureInit(const char *name) {
  if (!_roblexBleInited) {
    BLEDevice::init(name);
    _roblexBleInited = true;
  }
}

// Parser de la cadena "outR,outL,R,G,B" que envia la app.
void ROBLEX::ReadApp(String cmd) {
  int index[20];
  index[0] = -1;
  String value[20];
  bool last = false;
  int i = 1;
  while (!last) {
    index[i] = cmd.indexOf(',', index[i - 1] + 1);
    ROBLEX::AppValue[i - 1] = cmd.substring(index[i - 1] + 1, index[i]);
    if (index[i] < 0) {
      last = true;
    }
    i++;
  }
}

// Callbacks de conexion del servidor BLE.
class _RoblexServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) {
    _roblexConnected = true;
  }
  void onDisconnect(BLEServer *server) {
    _roblexConnected = false;
    server->getAdvertising()->start();  // volver a anunciarse para reconectar
  }
};

// Callback al recibir datos en la caracteristica RX: acumula hasta '\n' y
// deja lista la ultima linea completa.
class _RoblexRxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    auto value = characteristic->getValue();
    for (size_t i = 0; i < value.length(); i++) {
      char c = value[i];
      if (c == '\n') {
        _roblexRxLine = _roblexRxBuffer;
        _roblexLineReady = true;
        _roblexRxBuffer = "";
      } else if (c != '\r') {
        _roblexRxBuffer += c;
      }
    }
  }
};

void ROBLEX::BluetoothBegin(String name) {
  _roblexEnsureInit(name.c_str());
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new _RoblexServerCallbacks());

  BLEService *service = server->createService(ROBLEX_BLE_SERVICE);

  // TX (robot -> app): notificaciones, para telemetria.
  _roblexTx = service->createCharacteristic(
      ROBLEX_BLE_TX, BLECharacteristic::PROPERTY_NOTIFY);
  _roblexTx->addDescriptor(new BLE2902());

  // RX (app -> robot): recibe los comandos.
  BLECharacteristic *rx = service->createCharacteristic(
      ROBLEX_BLE_RX,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  rx->setCallbacks(new _RoblexRxCallbacks());

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(ROBLEX_BLE_SERVICE);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();
}

bool ROBLEX::BluetoothAvailable(void) {
  return _roblexLineReady;
}

String ROBLEX::BluetoothRead(void) {
  _roblexLineReady = false;
  return _roblexRxLine;
}

bool ROBLEX::BluetoothConnected(void) {
  return _roblexConnected;
}

void ROBLEX::BluetoothWrite(String message) {
  if (_roblexConnected && _roblexTx != nullptr) {
    _roblexTx->setValue(message.c_str());
    _roblexTx->notify();
  }
}

String ROBLEX::BluetoothAddress(void) {
  // Direccion (MAC) BLE de este dispositivo. Valida tras BluetoothBegin().
  return BLEDevice::getAddress().toString();
}

// ===================================================================
//  Lado CONTROL REMOTO: cliente BLE que se conecta al robot (servidor)
// ===================================================================

static BLEClient *_roblexClient = nullptr;
static BLERemoteCharacteristic *_roblexRemoteRx = nullptr;
static BLEAdvertisedDevice *_roblexFound = nullptr;
static bool _roblexClientConnected = false;
static String _roblexTargetName = "";

class _RoblexClientCallbacks : public BLEClientCallbacks {
  void onConnect(BLEClient *c) {}
  void onDisconnect(BLEClient *c) {
    _roblexClientConnected = false;
  }
};

// Durante el escaneo, busca un robot que anuncie el servicio NUS (y, si se
// indico, que coincida con el nombre).
class _RoblexScanCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice dev) {
    if (dev.haveServiceUUID() &&
        dev.isAdvertisingService(BLEUUID(ROBLEX_BLE_SERVICE))) {
      if (_roblexTargetName.length() == 0 || dev.getName() == _roblexTargetName) {
        if (_roblexFound == nullptr) {
          _roblexFound = new BLEAdvertisedDevice(dev);
        }
        dev.getScan()->stop();
      }
    }
  }
};

bool ROBLEX::BluetoothConnectRobot(String name) {
  _roblexEnsureInit("");  // inicializa BLE como central (cliente)
  _roblexTargetName = name;
  if (_roblexFound != nullptr) {
    delete _roblexFound;
    _roblexFound = nullptr;
  }

  // Escanear 5 segundos buscando el robot
  BLEScan *scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new _RoblexScanCallbacks());
  scan->setActiveScan(true);
  scan->setInterval(100);
  scan->setWindow(99);
  scan->start(5, false);
  scan->clearResults();

  if (_roblexFound == nullptr) return false;

  // Conectar y descubrir la caracteristica RX del robot
  _roblexClient = BLEDevice::createClient();
  _roblexClient->setClientCallbacks(new _RoblexClientCallbacks());

  if (!_roblexClient->connect(_roblexFound)) return false;

  BLERemoteService *service = _roblexClient->getService(BLEUUID(ROBLEX_BLE_SERVICE));
  if (service == nullptr) {
    _roblexClient->disconnect();
    return false;
  }
  _roblexRemoteRx = service->getCharacteristic(BLEUUID(ROBLEX_BLE_RX));
  if (_roblexRemoteRx == nullptr) {
    _roblexClient->disconnect();
    return false;
  }

  _roblexClientConnected = true;
  return true;
}

bool ROBLEX::BluetoothConnectAddress(String mac) {
  _roblexEnsureInit("");  // inicializa BLE como central (cliente)

  // Conexion directa a la MAC unica del robot (sin escaneo por nombre).
  _roblexClient = BLEDevice::createClient();
  _roblexClient->setClientCallbacks(new _RoblexClientCallbacks());

  if (!_roblexClient->connect(BLEAddress(mac))) return false;

  BLERemoteService *service = _roblexClient->getService(BLEUUID(ROBLEX_BLE_SERVICE));
  if (service == nullptr) {
    _roblexClient->disconnect();
    return false;
  }
  _roblexRemoteRx = service->getCharacteristic(BLEUUID(ROBLEX_BLE_RX));
  if (_roblexRemoteRx == nullptr) {
    _roblexClient->disconnect();
    return false;
  }

  _roblexClientConnected = true;
  return true;
}

bool ROBLEX::BluetoothRobotConnected(void) {
  return _roblexClientConnected && _roblexClient != nullptr &&
         _roblexClient->isConnected();
}

void ROBLEX::BluetoothSend(String message) {
  if (_roblexRemoteRx != nullptr && BluetoothRobotConnected()) {
    String m = message + "\n";
    _roblexRemoteRx->writeValue(m, false);  // write sin respuesta
  }
}

// ===================================================================
//  Escaneo de robots (para construir un buscador/menu en el control)
// ===================================================================

#define ROBLEX_MAX_SCAN 10
static String _scanNames[ROBLEX_MAX_SCAN];
static String _scanAddrs[ROBLEX_MAX_SCAN];
static int _scanCount = 0;

// Recoge robots unicos que anuncian el servicio NUS.
class _RoblexScanListCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice dev) {
    if (!dev.haveServiceUUID() ||
        !dev.isAdvertisingService(BLEUUID(ROBLEX_BLE_SERVICE)))
      return;

    String addr = dev.getAddress().toString();
    for (int i = 0; i < _scanCount; i++) {
      if (_scanAddrs[i] == addr) return;  // ya esta en la lista
    }
    if (_scanCount < ROBLEX_MAX_SCAN) {
      _scanAddrs[_scanCount] = addr;
      String n = dev.getName();
      _scanNames[_scanCount] = (n.length() > 0) ? n : addr;
      _scanCount++;
    }
  }
};

static _RoblexScanListCallbacks _scanListCb;

int ROBLEX::BluetoothScan(int seconds) {
  _roblexEnsureInit("");
  _scanCount = 0;

  BLEScan *scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(&_scanListCb);
  scan->setActiveScan(true);
  scan->setInterval(100);
  scan->setWindow(99);
  scan->start(seconds, false);
  scan->clearResults();

  return _scanCount;
}

String ROBLEX::BluetoothScanName(int index) {
  if (index < 0 || index >= _scanCount) return "";
  return _scanNames[index];
}

String ROBLEX::BluetoothScanAddress(int index) {
  if (index < 0 || index >= _scanCount) return "";
  return _scanAddrs[index];
}
