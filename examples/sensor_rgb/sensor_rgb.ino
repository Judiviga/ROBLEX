#include "ROBLEX.h"
#include "Adafruit_TCS34725.h"

ROBLEX ROBLEX;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(115200);
  Serial.println("start");
  tcs.begin();

}

void loop() {
  // tcs.setInterrupt(true);  // turn off LED
  float red, green, blue;
  tcs.getRGB(&red, &green, &blue);

  Serial.print(int(blue));
  Serial.print(" ");
  Serial.print(int(green));
  Serial.print(" ");
  Serial.print(int(red));
  Serial.println();

  ROBLEX.RGBled(int(red), int(green), int(blue));
}
