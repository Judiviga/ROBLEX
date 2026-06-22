#include "ROBLEX.h"

// Ejemplo reproducir melodia y notas con el modulo buzzer
// melodias disponible en: https://github.com/robsoncouto/arduino-songs
// el modulo buzzer cuenta con el buzzer en el pinA y un boton en el pinB

// CONECTAR EL MODULO BUZZER EN EL PUERTO 3

#define BUZZER_PIN pin3A  // definir el buzzer en el puerto 3A
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

  pinMode(BOTON_PIN, INPUT);       // configurar el boton como entrada
  ROBLEX.BuzzerBegin(BUZZER_PIN);  // configurar el buzzer
}


void loop() {
  // reproducir melodias
  if (digitalRead(BOTON_PIN) == 1) {          // reproducir cuando se presiona el boton del modulo
    int n = sizeof(melody) / sizeof(melody[0]);  // calcular el tamano de la melodia
    ROBLEX.PlayMelody(melody, n, tempo);         // reproducir la melodia (melodia, tamano de la melodia, velocidad)
  }

  // reproducir notas sueltas
  if (digitalRead(boton)== 1) {                   // reproducir cuando se presiona el boton de la shield
    ROBLEX.PlayTone(NOTE_E5);  // reproducir nota E5
    delay(200);
    ROBLEX.PlayTone(NOTE_G5);  // reproducir nota G5
    delay(200);
    ROBLEX.NoTone();  // silencio
    delay(200);
  } 
}
