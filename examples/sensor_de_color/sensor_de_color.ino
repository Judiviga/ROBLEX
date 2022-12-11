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
 

  if (analogRead(boton)) {   //encender el led RGB si se presiona el boton de la shield
    tcs.setInterrupt(true);  // turn off LED
  } else {
    tcs.setInterrupt(false);  // turn on LED
  }

  float r, g, b;
  tcs.getRGB(&r, &g, &b);

  Serial.print(int(b));
  Serial.print(" ");
  Serial.print(int(g));
  Serial.print(" ");
  Serial.print(int(r));
  Serial.println();

  ROBLEX.Rgb(int(r), int(g), int(b));
}