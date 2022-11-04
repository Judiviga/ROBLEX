#include "ROBLEX.h"

// address we will assign for each sensor
#define front_adress 0x30
#define left_adress 0x32
#define right_adress 0x31

// objects for the vl53l0x
Adafruit_VL53L0X front = Adafruit_VL53L0X();
Adafruit_VL53L0X left = Adafruit_VL53L0X();
Adafruit_VL53L0X right = Adafruit_VL53L0X();


bool aL = false;
bool aF = false;
bool aR = false;

void ROBLEX::SetupDistance(uint8_t port , bool activeL, bool activeF, bool activeR) {

  int * p = ROBLEX::GetPins(port);
  int left_sht = p[0];
  int right_sht = p[1];

  pinMode(left_sht, OUTPUT);
  pinMode(right_sht, OUTPUT);

  aL = activeL;
  aR = activeR;
  aF = activeF;

  //shutting right and left
  digitalWrite(left_sht, LOW);
  digitalWrite(right_sht, LOW);

  if (activeF) {
    front.begin(front_adress, false,  &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);  // initing front
    delay(10);
    front.startRangeContinuous();
  }
  if (activeR) {
    digitalWrite(right_sht, HIGH);  // activating right
    delay(10);
    right.begin(right_adress, false,  &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_ACCURACY);     //initing right
    delay(10);
    right.startRangeContinuous();
  }
  if (activeL) {
    digitalWrite(left_sht, HIGH);   //activating left
    delay(10);
    left.begin(left_adress, false,  &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_ACCURACY);    //initing left
    delay(10);
    left.startRangeContinuous();
  }

}

uint16_t ROBLEX::LeftRange(void) {
  if (aL) {
    return left.readRange();
  } else {
    return 0;
  }
}

uint16_t ROBLEX::FrontRange(void) {
  if (aF) {
    return front.readRange();
  } else {
    return 0;
  }
}

uint16_t ROBLEX::RightRange(void) {
  if (aR) {
    return right.readRange();
  } else {
    return 0;
  }
}

bool ROBLEX::RightComplete(void) {
  return right.isRangeComplete();
}

bool ROBLEX::LeftComplete(void) {
  return left.isRangeComplete();
}

bool ROBLEX::FrontComplete(void) {
  return front.isRangeComplete();
}

uint8_t ROBLEX::RightStatus(void) {
   return right.readRangeStatus();
}
uint8_t ROBLEX::LeftStatus(void) {
   return left.readRangeStatus();
}
uint8_t ROBLEX::FrontStatus(void) {
   return front.readRangeStatus();
}