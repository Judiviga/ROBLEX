#include "COBRA.h"

Cobra Cobra;

#define LEFT_PWM   MCPWM_UNIT_0
#define RIGHT_PWM   MCPWM_UNIT_1

void setup() {

  Cobra.BtBegin("NEW COBRA");
  Serial.begin(115200);

  Cobra.SetupMotor(4, LEFT_PWM, 20000);
  Cobra.SetupMotor(5, RIGHT_PWM, 20000);
}


void Drive(mcpwm_unit_t unit, int out) {

  if (out < 0) {
    Cobra.MotorForward(unit, abs(out));

  } else if (out > 0) {
    Cobra.MotorBackward(unit, out);

  } else {
    Cobra.MotorStop(unit);
  }
}

void loop() {

  Cobra.ReadApp();

  Drive(RIGHT_PWM,  Cobra.AppValue[0].toInt());
  Drive(LEFT_PWM, Cobra.AppValue[1].toInt());
  Cobra.RGBled(Cobra.AppValue[2].toInt(), Cobra.AppValue[3].toInt(), Cobra.AppValue[4].toInt());

}
