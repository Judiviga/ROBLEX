#include "ROBLEX.h"

// Ejemplo leer un modulo de sensor analogo y graficar en la pantalla oled y el debug serial
// enciende el led del modulo si el valor analogo es bajo
// es compatible con los modulos:

// Sensor de luz
// Sensor de sonido
// Potenciometro
// Botones

// estos modulos tiene el sensor en el pin A y un led en el pinB

// CONECTAR EL MODULO DE SENSOR ANALOGO EN EL PUERTO 2 Y LA PANTALLA EN CUALQUIER OTRO

#define SENSOR_PIN pin2A  // definir el sensor en el puerto 2A
#define LED_PIN pin2B     // definir el led en el puerto 2B


ROBLEX ROBLEX;

int cuenta;

void IRAM_ATTR encoder() {

  if (digitalRead(SENSOR_PIN) == digitalRead(LED_PIN)) {
    cuenta++;
  } else {
    cuenta--;
  }
}


void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  pinMode(SENSOR_PIN, INPUT);  // configura el puerto A como entrada
  pinMode(LED_PIN, INPUT);     // configura el puerto B como salida

  attachInterrupt(SENSOR_PIN, encoder, FALLING);
}


void loop() {


  Serial.print(digitalRead(SENSOR_PIN));  // imprime el valor en el debug serial
  Serial.print("  ");
  Serial.print(digitalRead(LED_PIN));
  Serial.print("      ");
  Serial.print(cuenta);
  Serial.println();
}