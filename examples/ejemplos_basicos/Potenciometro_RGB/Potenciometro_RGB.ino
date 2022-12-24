#include "ROBLEX.h"

// Ejemplo leer el modulo potenciometro para varia la intensidad del led RGB
//CONECTAR EL MODULO POTENCIOMETRO EN EL PUERTO 2

#define POTENCIOMETRO pin2A  //Asignamos el pin 2A al potenciometro

ROBLEX ROBLEX;

int lectura;  //Declaramos dos variables enteras para almacenar la lectura del potenciometro y convertirla
int mapeo;

void setup() {
  Serial.begin(115200);
  pinMode(POTENCIOMETRO, INPUT);  //Declaramos el potenciometro como una entrada
}

void loop() {
  lectura = analogRead(POTENCIOMETRO);  //Almacenamos la lectura del potenciometro en una variable entera

  Serial.println(lectura); //imprimir el valor en el debug serial

  mapeo = map(lectura, 0, 4095, 0, 255);  //Convertimos los valores del potenciometro al rango adecuado para el RGB y los guardamos en otra variable

  ROBLEX.Rgb(mapeo, mapeo, mapeo);  //Asignamos el valor del potenciometro a los tres colores del RGB para variar su intensidad
}

