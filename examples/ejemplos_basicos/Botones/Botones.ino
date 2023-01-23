#include "ROBLEX.h"

// Ejemplo leer el modulo Botones y enciende el led RGB y el led del modulo segun el boton presionado
// CONECTAR EL MODULO DE BOTONES EN EL PUERTO 5 

#define SENSOR_PIN pin5A  // definir el sensor en el puerto 5A
#define LED_PIN pin5B     // definir el led en el puerto 5B

ROBLEX ROBLEX;

void setup() {

  Serial.begin(115200);  // iniciar debug serial

  pinMode(SENSOR_PIN, INPUT);  //define el pin A como entrada
  pinMode(LED_PIN, OUTPUT);    //define el pin B como salida

}

// define valores numericos a los valores para facilitar la interpretacion
#define ARRIBA 4
#define DERECHA 3
#define ABAJO 2
#define IZQUIERDA 1
#define NINGUNO 0

//crea una funcion que identifica el boton presionado segun su valor
int botones(int valor) {

 if (valor > 4000) {
    return ARRIBA;
  } else if (valor > 3000) {
    return DERECHA;
  } else if (valor > 1000) {
    return ABAJO;
  } else if (valor > 500) {
    return IZQUIERDA;
  } else {
    return NINGUNO;
  }
}

void loop() {

  int valor = analogRead(SENSOR_PIN);  // lee el valor del sensor analogico

  // obtiene el boton presionado haciendo uso de la funcion creada anteriormente
  int presionado = botones(valor);

  // realizar funciones segun el boton presionado
  if (presionado == ARRIBA) {  // encender el led del modulo de botones si se presiona el boton de arriba
    digitalWrite(LED_PIN, 255);
    Serial.print("ARRIBA");
  } else {
    digitalWrite(LED_PIN, 0);
  }

  if (presionado == DERECHA) {  // cambiar el color del led rgb con los otros botones
    ROBLEX.Rgb(255, 0, 0);      //rojo
    Serial.print("DERECHA");
  } else if (presionado == ABAJO) {
    ROBLEX.Rgb(0, 255, 0);  //verde
    Serial.print("ABAJO");
  } else if (presionado == IZQUIERDA) {
    ROBLEX.Rgb(0, 0, 255);  //azul
    Serial.print("IZQUIERDA");
  } else {
    ROBLEX.Rgb(0, 0, 0);
  }
  if (presionado == NINGUNO) {
    Serial.print("NINGUNO");
  }

  Serial.println();


}