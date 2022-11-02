#include "ROBLEX.h"

// Ejemplo reproducir melodia con el modulo buzzer
// melodias disponible en: https://github.com/robsoncouto/arduino-songs
//el modulo buzzer cuenta con el buzzer en el pin A y un boton en el pinB

// CONECTAR EL MODULO BUZZER EN EL PUERTO 3

#define BUZZZER_PIN pin3A  // definir el buzzer en el puerto 3A
#define BOTON_PIN pin3B    // definir el boton en el puerto 3B


ROBLEX ROBLEX;

int melody[] = {
  // Super Mario Bros theme
  // Disponible en https://github.com/robsoncouto/arduino-songs/blob/master/supermariobros/supermariobros.ino
  // Theme by Koji Kondo
  NOTE_E5, 8, NOTE_E5, 8, REST, 8, NOTE_E5, 8, REST, 8, NOTE_C5, 8, NOTE_E5, 8,
  REST, 8, NOTE_G5, 4
};

int tempo = 200;  //velocidad de reproduccion de la melodia tomada de la misma pagina

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  pinMode(BOTON_PIN, INPUT);      //configurar el boton como entrada
  ledcAttachPin(BUZZZER_PIN, 0);  //configurar el buzzer como salida PWM en el canal 0
}


void loop() {
  if (digitalRead(BOTON_PIN) == HIGH) {  //reproducir cuando se presion el boton
    unsigned int n = sizeof(melody) / sizeof(melody[0]); //calcular el tamano de la melodia
    ROBLEX.PlayMelody(melody, n, tempo);  //reproducir la melodia (melodia, tamano de la melodia, velocidad)
  }

}