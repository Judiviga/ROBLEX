#include "ROBLEX.h"
#include <Adafruit_PCF8574.h>

ROBLEX ROBLEX;
Adafruit_PCF8574 pcf;
void setup() {
  Serial.begin(115200);
  Serial.println("start");
  pcf.begin(0x27, &Wire);

  for (uint8_t p = 0; p < 8; p++) {
    pcf.pinMode(p, OUTPUT);
  }

}

void loop() {
  for (uint8_t p = 0; p < 8; p++) {
    pcf.digitalWrite(p, LOW);  // turn LED on by sinking current to ground
    delay(100);
    pcf.digitalWrite(p, HIGH); // turn LED off by turning off sinking transistor
  }
}
/*

  #include <Arduino.h>
  #include <Wire.h>

  void setup()
  {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial);
  Serial.println("\nI2C Scanner");
  }

  void loop()
  {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
  // The i2c_scanner uses the return value of
  // the Write.endTransmisstion to see if
  // a device did acknowledge to the address.
  Wire.beginTransmission(address);
  error = Wire.endTransmission();
  if (error == 0)
  {
    Serial.print("I2C device found at address 0x");
    if (address < 16)
      Serial.print("0");
    Serial.print(address, HEX);
    Serial.println("  !");
    nDevices++;
  }
  else if (error == 4)
  {
    Serial.print("Unknown error at address 0x");
    if (address < 16)
      Serial.print("0");
    Serial.println(address, HEX);
  }
  }
  if (nDevices == 0)
  Serial.println("No I2C devices found\n");
  else
  Serial.println("done\n");
  delay(5000);           // wait 5 seconds for next scan
  }*/
