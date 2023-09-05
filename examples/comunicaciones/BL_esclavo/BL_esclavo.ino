#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ROBLEX SLAVE"); //Nombre del dispositivo
  
  Serial.println(SerialBT.getBtAddressString()); //imprimir el address del dispositivo
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
}
