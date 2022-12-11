#include "ROBLEX.h"

// Ejemplo mover dos servomotores cuando se presiona el boton de la shield

// CONNECTAR EL MODULO SERVOMOTORES EN EL PUERTO 3

#define SERVOA_PIN pin3A  // definir el servomotor A en el pin 3A
#define SERVOB_PIN pin3B  // definir el servomotor B en el pin 3B

ROBLEX ROBLEX;

#include <ESP32Servo.h>  // importar libreria ESP32Servo

Servo servoA;  // crear objeto servo A
Servo servoB;  // crear objeto servo B

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  servoA.attach(SERVOA_PIN, 500, 2400);  // iniciar los servomotores en los puertos respectivos
  servoB.attach(SERVOB_PIN, 500, 2400);  // asignar un minimo de 500us y maximo de 2400us de pulso segun las especificaciones de servomotor SG90
}


void loop() {

  if (digitalRead(boton)) {     //encender el led RGB si se presiona el boton de la shield
    servoA.write(180);          // mover el servo A a la posicion 180 grados
    servoB.write(0);            // mover el servo B a la posicion 0 grados
  } else {
    servoA.write(0);      // mover el servo B a la posicion 0 grados
    servoB.write(180);    // mover el servo A a la posicion 180 grados
  }
}