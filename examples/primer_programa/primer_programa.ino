#include "ROBLEX.h"

// Ejemplo basico para probar el funcionamiento de la shield ROBLEX
// Enciende el led RGB en color blanco cuando se presiona el boton de la shield

// NO REQUIERE NINGUN MODULO

ROBLEX ROBLEX;

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");
}


void loop() {

  if (analogRead(boton)) {      //encender el led RGB si se presiona el boton de la shield
    ROBLEX.Rgb(255, 255, 255);  //blanco
  } else {
    ROBLEX.Rgb(0, 0, 0);  //apagado
  }
}