/*
  Blink led on PIN0
  by Mischianti Renzo <http://www.mischianti.org>

  https://www.mischianti.org/2019/01/02/pcf8574-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF8574.h"

// Set i2c address
PCF8574 pcf8574(0x20);

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Set pinMode to OUTPUT
  pcf8574.pinMode(P0, OUTPUT);
  pcf8574.pinMode(P1, OUTPUT);
  pcf8574.pinMode(P2, OUTPUT);
  pcf8574.pinMode(P3, OUTPUT);
  pcf8574.pinMode(P4, OUTPUT);
  pcf8574.pinMode(P5, OUTPUT);
  pcf8574.pinMode(P6, OUTPUT);

  Serial.print("Init pcf8574...");
  if (pcf8574.begin()) {
    Serial.println("OK");
  } else {
    Serial.println("KO");
  }
}

void loop()
{
 // pcf8574.digitalWrite(P0, HIGH);
  pcf8574.digitalWrite(P1, HIGH);
 // pcf8574.digitalWrite(P2, HIGH);
  pcf8574.digitalWrite(P3, HIGH);
  //pcf8574.digitalWrite(P4, HIGH);
  pcf8574.digitalWrite(P5, HIGH);
 // pcf8574.digitalWrite(P6, HIGH);
  Serial.println("on");
  delay(1000);
  pcf8574.digitalWrite(P0, LOW);
  pcf8574.digitalWrite(P1, LOW);
  pcf8574.digitalWrite(P2, LOW);
  pcf8574.digitalWrite(P3, LOW);
  pcf8574.digitalWrite(P4, LOW);
  pcf8574.digitalWrite(P5, LOW);
  pcf8574.digitalWrite(P6, LOW);

  Serial.println("off");
  delay(1000);
}
