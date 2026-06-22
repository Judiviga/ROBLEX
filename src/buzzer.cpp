#include "ROBLEX.h"

// Pin del buzzer (se fija con BuzzerBegin). En el core ESP32 3.x el LEDC se
// direcciona por PIN, no por canal.
static int _roblexBuzzerPin = -1;

void ROBLEX::BuzzerBegin(int pin) {
  _roblexBuzzerPin = pin;
  ledcAttach(pin, 2000, 8);  // adjunta el pin al LEDC (freq inicial, 8 bits)
}

void ROBLEX::PlayTone(int freq) {
  if (_roblexBuzzerPin >= 0) ledcWriteTone(_roblexBuzzerPin, freq);
}

void ROBLEX::NoTone(void) {
  if (_roblexBuzzerPin >= 0) ledcWriteTone(_roblexBuzzerPin, 0);
}

void ROBLEX::PlayMelody(int *melody, unsigned int n, int tempo) {
  int notes = n / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;
  int divider = 0, noteDuration = 0;

  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;  // increases the duration in half for dotted notes
    }
    // we only play the note for 90% of the duration, leaving 10% as a pause
    PlayTone(melody[thisNote]);
    delay(noteDuration * 0.9);

    NoTone();
    delay(noteDuration * 0.1);
    // stop the waveform generation before the next note.
  }
}
