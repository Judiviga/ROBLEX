#include "ROBLEX.h"

ROBLEX::ROBLEX() {
  // Anything you need when instantiating your object goes here
  pinMode(pinRojo, OUTPUT);
  pinMode(pinVerde, OUTPUT);
  pinMode(pinAzul, OUTPUT);
  Wire.begin();
  ledcSetup(0, 2000, 8);
  ledcSetup(1, 30000, 8);
  ledcSetup(2, 30000, 8);
  ledcSetup(3, 30000, 8);
  ledcSetup(4, 30000, 8);
}

void ROBLEX::Rgb(int R , int G , int B) {
  analogWrite(pinRojo, R);
  analogWrite(pinVerde, G);
  analogWrite(pinAzul, B);
}

void ROBLEX::Drive(int pwm, int chA , int chB) {
  if (pwm < 0) {

    ledcWrite(chA, -pwm);
    ledcWrite(chB, 0);
    /*
      analogWrite(chA, -pwm);
      analogWrite(chB, 255);
    */

  } else { //  forward

    ledcWrite(chA, 0);
    ledcWrite(chB, pwm);
    /*
        analogWrite(chA, 255);
        analogWrite(chB, pwm);
    */
  }
}

int * ROBLEX::GetPins(uint8_t port) {
  static int pin[2];
  if (port == 1) {
    pin[0] = pin1A;
    pin[1] = pin1B;
  }
  else if (port == 2) {
    pin[0] = pin2A;
    pin[1] = pin2B;
  }
  else if (port == 3) {
    pin[0] = pin3A;
    pin[1] = pin3B;
  }
  else if (port == 4) {
    pin[0] = pin4A;
    pin[1] = pin4B;
  }
  else if (port == 5) {
    pin[0] = pin5A;
    pin[1] = pin5B;
  } else {
    Serial.print("SetupPort error");
  }
  return pin;
}



void ROBLEX::SetupPort(uint8_t port, uint8_t mode1 , uint8_t mode2) {
  //INPUT          0
  //OUTPUT         1
  //INPUT_PULLUP   2

  int * p = ROBLEX::GetPins(port);
  pinMode(p[0], mode1);
  pinMode(p[1], mode2);
}


// 'logo roblex', 128x64px
unsigned char ROBLEX::LogoRoblex [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x18, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x18, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xf0, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xfc, 0x00, 0xfe, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xff, 0x01, 0xff, 0x87, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xff, 0x83, 0xef, 0xc7, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x03, 0xc7, 0x83, 0xc7, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x01, 0xcf, 0x01, 0xe7, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x01, 0xce, 0x00, 0xe7, 0x03, 0x81, 0x00, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x01, 0xce, 0x00, 0xe7, 0x07, 0x83, 0x80, 0x3f, 0xe1, 0xc0, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x03, 0xce, 0x00, 0xe7, 0xff, 0x03, 0x80, 0x3f, 0xe1, 0xc1, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x03, 0x8e, 0x00, 0xe7, 0xff, 0x03, 0x80, 0x3f, 0xe0, 0xe1, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x07, 0x8e, 0x00, 0xe7, 0xff, 0x83, 0x80, 0x38, 0x00, 0xf3, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xff, 0x0e, 0x00, 0xe7, 0x03, 0xc3, 0x80, 0x38, 0x00, 0x77, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xfe, 0x0e, 0x00, 0xe7, 0x01, 0xc3, 0x80, 0x3f, 0x80, 0x3f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xfc, 0x0f, 0x01, 0xe7, 0x01, 0xc3, 0x80, 0x3f, 0xc0, 0x3e, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x1c, 0x07, 0x81, 0xc7, 0x01, 0xc3, 0x80, 0x3f, 0xc0, 0x1e, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x0e, 0x07, 0xc7, 0xc7, 0x87, 0xc3, 0x80, 0x38, 0x00, 0x1e, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x0f, 0xc3, 0xff, 0x87, 0xff, 0x83, 0x80, 0x38, 0x00, 0x3e, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x07, 0xc0, 0xfe, 0x07, 0xff, 0x03, 0x80, 0x38, 0x00, 0x7f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x80, 0x10, 0x00, 0x00, 0x03, 0x80, 0x38, 0x00, 0x77, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x38, 0x00, 0xf3, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x38, 0x01, 0xe1, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x3f, 0xf1, 0xc1, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x3f, 0xfb, 0xc0, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x3f, 0xf1, 0x80, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x18, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x38, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xf0, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00
};
