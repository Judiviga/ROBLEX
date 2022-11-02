#include "COBRA.h"

Cobra Cobra;

#define BUZZZER_PIN  pin5A // ESP32 pin GIOP18 connected to piezo buzzer

int melody[] = {

  // Fur Elise - Ludwig van Beethovem
  // Score available at https://musescore.com/user/28149610/scores/5281944

  //starts from 1 ending on 9
  NOTE_E5, 16, NOTE_DS5, 16, //1
  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,
  NOTE_C5, 8,  REST, 16, NOTE_E4, 16, NOTE_E5, 16,  NOTE_DS5, 16,

  NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,//6
  NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16,
  NOTE_B4, -8, NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16,
  NOTE_A4 , 4, REST, 8, //9 - 1st ending

};

int  tempo = 80;
void setup() {

  Cobra.SetupPort(5, OUTPUT, INPUT);
  ledcAttachPin(BUZZZER_PIN, 0);

  Serial.begin(115200);
}


void loop() {
  if (digitalRead(pin5B) == LOW) {
    unsigned int n = sizeof(melody) / sizeof(melody[0]);
    Cobra.PlayMelody(melody, n, tempo);
  }

}
