#include "ROBLEX.h"
#include <Adafruit_PCF8574.h> //importar libreria PCF8574

// Ejemplo uso del modulo puertos digitales para encender secuencia de leds 

// CONECTAR EL MODULO PUERTOS DIGITALES EN CUALQUIER PUERTO

ROBLEX ROBLEX;  // llamar libreria ROBLEX 
Adafruit_PCF8574 pcf; // llamar libreria pcf

void setup() {

  Serial.begin(115200); // iniciar debug serial
  Serial.println("start");

  pcf.begin(0x20, &Wire); // iniciar pcf 

  for (int p = 1; p <= 6; p++) {  // configurar los 6 puertos como salidas
    pcf.pinMode(p, OUTPUT);
  }

}

void loop() {

  for (int p = 1; p <= 6; p++) {
    pcf.digitalWrite(p, HIGH);  // encender el led respectivo en la secuencia
    delay(100);                 // tiempo de encendido de cada led
    pcf.digitalWrite(p, LOW);   // apagar el led
  }
}