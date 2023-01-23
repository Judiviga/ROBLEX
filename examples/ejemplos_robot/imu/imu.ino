#include "ROBLEX.h"
#include <MPU6050_tockn.h>
#include <Wire.h>

// Ejemplo basico para probar el funcionamiento de la shield ROBLEX
// Enciende el led RGB en color blanco cuando se presiona el boton de la shield

// NO REQUIERE NINGUN MODULO

ROBLEX ROBLEX;
MPU6050 imu(Wire);

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  imu.begin();
  imu.calcGyroOffsets(true);
}


void loop() {
  imu.update();



  Serial.print("temp : ");
  Serial.println(imu.getTemp());

  Serial.print("accX : ");
  Serial.print(imu.getAccX());
  Serial.print("\taccY : ");
  Serial.print(imu.getAccY());
  Serial.print("\taccZ : ");
  Serial.println(imu.getAccZ());

  Serial.print("gyroX : ");
  Serial.print(imu.getGyroX());
  Serial.print("\tgyroY : ");
  Serial.print(imu.getGyroY());
  Serial.print("\tgyroZ : ");
  Serial.println(imu.getGyroZ());

  Serial.print("accAngleX : ");
  Serial.print(imu.getAccAngleX());
  Serial.print("\taccAngleY : ");
  Serial.println(imu.getAccAngleY());

  Serial.print("gyroAngleX : ");
  Serial.print(imu.getGyroAngleX());
  Serial.print("\tgyroAngleY : ");
  Serial.print(imu.getGyroAngleY());
  Serial.print("\tgyroAngleZ : ");
  Serial.println(imu.getGyroAngleZ());

  Serial.print("angleX : ");
  Serial.print(imu.getAngleX());
  Serial.print("\tangleY : ");
  Serial.print(imu.getAngleY());
  Serial.print("\tangleZ : ");
  Serial.println(imu.getAngleZ());
  Serial.println();
  delay(100);
}