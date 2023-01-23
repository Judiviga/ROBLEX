#include "ROBLEX.h"

// Ejemplo leer el modulo switch para cambiar el color del led RGB
//CONECTAR EL MODULO SWITCH EN EL PUERTO 1

//definimos el SWITCH en el puerto 1
#define SWITCH_A pin1A
#define SWITCH_B pin1B  

ROBLEX ROBLEX;

int A;
int B;  //Declaramos dos variables para almacenar el estado del switch

//Declaramos los switches como entradas
void setup() {
  pinMode(SWITCH_A, INPUT);
  pinMode(SWITCH_B, INPUT);  
}

void loop() {

  A = digitalRead(SWITCH_A);
  B = digitalRead(SWITCH_B);

  if (A == 0 && B == 0) {
    ROBLEX.Rgb(0, 0, 0);  //RGB Apagado
  }

  if (A == 1 && B == 0) {
    ROBLEX.Rgb(255, 0, 0);  //Rojo
  }

  if (A == 0 && B == 1) {
    ROBLEX.Rgb(0, 255, 0);  //Verde
  }

  if (A == 1 && B == 1) {
    ROBLEX.Rgb(0, 0, 255);  //Azul
  }
}
