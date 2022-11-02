#include "ROBLEX.h"


ROBLEX ROBLEX;

long encoder = 0;

void IRAM_ATTR ISR() {
  if (digitalRead(pin1B) == digitalRead(pin1A) ) {
    encoder = encoder - 1;
  }
  else {
    encoder = encoder + 1;
  }
  Serial.println(encoder);
}



void setup() {
  Serial.begin(115200);
  Serial.println("start");
  ROBLEX.SetupPort(1, INPUT, INPUT);
  attachInterrupt(pin1A, ISR, FALLING);//setup
}

void loop() {


  Serial.print(encoder);
  Serial.print("   ");
  Serial.print(digitalRead(pin1B));
  Serial.print("   ");
  Serial.print(digitalRead(pin1A));
  Serial.println();
  delay(50);
}
