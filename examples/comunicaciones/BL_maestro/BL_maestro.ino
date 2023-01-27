#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

uint8_t slaveAddress[] = { 0xC8, 0xF0, 0x0A, 0x0A, 0x0A, 0x0A };

void setup() {
  Serial.begin(115200);

  SerialBT.begin("ROBLEX MAESTRO", true);  //nombre de su dispositivo como maestro

  bool connected = SerialBT.connect(slaveAddress);

  if (connected) {
    Serial.println("Conexion exitosa!");
  } else {
    while (!SerialBT.connected(10000)) {
      Serial.println("Conexion fallida, intentando nuevamente...");
    }
  }
}

void loop() {

  //enviar y recibir Strings como maestro
  if (SerialBT.connected()) {

    if (Serial.available()) {
      SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
      Serial.write(SerialBT.read());
    }

  } else {
    SerialBT.connect(slaveAddress);
  }
  delay(50);
}
