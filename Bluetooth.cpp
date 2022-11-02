#include "ROBLEX.h"

BluetoothSerial SerialBT;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

void ROBLEX::BtBegin(String robotName) {
  SerialBT.begin(robotName); //Bluetooth device name
}


void  ROBLEX::ReadApp() {
  if (SerialBT.available()) {
    String  cmd = SerialBT.readStringUntil('\n');
    int index [20];
    index[0] = -1;
    String value [20];
    bool last = false;
    int i = 1;
    while (!last) {
      index[i] = cmd.indexOf(',', index[i - 1] + 1);
      ROBLEX::AppValue[i - 1] = cmd.substring(index[i - 1] + 1, index[i]);
      if ( index[i] < 0) {
        last = true;
      }
      i++;
    }
  }
   if (!SerialBT.connected(200)) {
    Serial.print("disconnected");
    ROBLEX::AppValue[0]="0";
    ROBLEX::AppValue[1]="0";
    ROBLEX::AppValue[2]="0";
    ROBLEX::AppValue[3]="0";
    ROBLEX::AppValue[4]="0";
  }
}
