#include "ROBLEX.h"
#include <Adafruit_PCF8574.h>  //importar libreria PCF8574

// Ejemplo uso del modulo puertos digitales para leer entradas
// se imprime en el debug serial el estado de los puertos

// CONECTAR EL MODULO PUERTOS DIGITALES EN CUALQUIER PUERTO

ROBLEX ROBLEX;         // llamar libreria ROBLEX
Adafruit_PCF8574 pcf;  // llamar libreria pcf

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  pcf.begin(0x20, &Wire);  // iniciar pcf

  for (int p = 1; p <= 6; p++) {
    pcf.pinMode(p, INPUT_PULLUP);  // configurar los 6 puertos como entradas encendidas por default
  }
}

void loop() {

  for (int p = 1; p <= 6; p++) {
    Serial.print(pcf.digitalRead(p));  // leer la entrada respectiva e imprimir en el debug serial
    Serial.print("   ");
  }
  Serial.println();
}