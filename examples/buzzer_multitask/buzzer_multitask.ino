#include "ROBLEX.h"

ROBLEX ROBLEX;

// Ejemplo uso del segundo nucleo del ESP32 para multitasking con el modulo Buzzer
// de esta forma no se detiene el programa y se pueden realizar otras tareas mientras se reproduce la melodia
// melodias disponible en: https://github.com/robsoncouto/arduino-songs
// el modulo Buzzer cuenta con el buzzer en el pinA y un boton en el pinB

// CONECTAR EL MODULO BUZZER EN EL PUERTO 3

#define BUZZZER_PIN pin3A  // definir el buzzer en el puerto 3A
#define BOTON_PIN pin3B    // definir el boton en el puerto 3B


int melody[] = {

  // Fur Elise - Ludwig van Beethovem

  NOTE_E5, 16, NOTE_DS5, 16,  //1
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,
  NOTE_C5, 8, REST, 16, NOTE_E4, 16, NOTE_E5, 16, NOTE_DS5, 16,

  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,  //6
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16,
  NOTE_A4, 4, REST, 8,  //9 - 1st ending

};

int tempo = 80;  // velocidad de reproduccion de la melodia tomada de la misma pagina

TaskHandle_t Task1;  // creas tarea adicional

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  pinMode(BOTON_PIN, INPUT);      //configurar el boton como entrada
  ledcAttachPin(BUZZZER_PIN, 0);  //configurar el buzzer como salida PWM en el canal 0

  //configuracion de la tarea en el nucleo secundario
  xTaskCreatePinnedToCore(
    Task1code,
    "Task1",
    10000,
    NULL,
    1,
    &Task1,
    0);
}

// loop de la funcion adicional
void Task1code(void* pvParameters) {
  while (true) {
    // reproducir melodias
    if (digitalRead(BOTON_PIN) == HIGH) {          // reproducir cuando se presiona el boton del modulo
      int n = sizeof(melody) / sizeof(melody[0]);  // calcular el tamano de la melodia
      ROBLEX.PlayMelody(melody, n, tempo);         // reproducir la melodia (melodia, tamano de la melodia, velocidad)
    }
    delay(20);
  }
}

void loop() {
  ROBLEX.Rgb(100, 0, 0);  // color rojo
  delay(1000);
  ROBLEX.Rgb(0, 100, 0);  // color verde
  delay(1000);
  ROBLEX.Rgb(0, 0, 100);  // color azul
  delay(1000);
}