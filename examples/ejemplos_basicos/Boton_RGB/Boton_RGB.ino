#include "ROBLEX.h"

// Ejemplo basico para probar el funcionamiento de la shield ROBLEX
// Enciende el led RGB en cualquier color cuando se presiona el boton de la shield

// NO REQUIERE NINGUN MODULO

ROBLEX ROBLEX;

void setup() {
  pinMode(boton, INPUT);
}

void loop() {

  if (digitalRead(boton) == 1) {  //encender el led RGB si se presiona el boton de la shield
    ROBLEX.Rgb(255, 255, 255);    //Utiliza el color que quieras! Cambia los valores del PWM de cada color
  } else {
    ROBLEX.Rgb(0, 0, 0);  //apagado
  }
}

// NOTA: Intenta cambiar el color del LED RGB modificando los valores de PWM que se encuentran en el codigo.
// ROBLEX.Rgb(R,G,B)
// Reemplaza los valores de R, G y B por los de tu color favorito utilizando el siguiente enlace:
// https://www.colorspire.com/rgb-color-wheel/