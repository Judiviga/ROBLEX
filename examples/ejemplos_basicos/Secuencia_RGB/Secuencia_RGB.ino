#include "ROBLEX.h"

// Ejemplo basico para probar el funcionamiento de la shield ROBLEX
// NO REQUIERE NINGUN MODULO

ROBLEX ROBLEX;

void setup() {
}

void loop() {

  //Encender el LED RGB de varios colores en una secuencia utilizando delay();
  ROBLEX.Rgb(255, 255, 255);  //blanco
  delay(1000);
  ROBLEX.Rgb(255, 0, 0);  //rojo
  delay(1000);
  ROBLEX.Rgb(0, 255, 0);  //verde
  delay(1000);
  ROBLEX.Rgb(0, 0, 255);  //azul
  delay(1000);
  ROBLEX.Rgb(0, 0, 0);  //apagado
  delay(1000);
}

// NOTA: Intenta cambiar el color del LED RGB modificando los valores de PWM que se encuentran en el codigo.
// ROBLEX.Rgb(R,G,B)
// Reemplaza los valores de R, G y B por los de tu color favorito utilizando el siguiente enlace:
// https://www.colorspire.com/rgb-color-wheel/