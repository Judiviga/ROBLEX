#include "ROBLEX.h"



void  ROBLEX::PlayMelody(int *melody, unsigned int n, int tempo) {
  int notes = n /  2;

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
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }
    // we only play the note for 90% of the duration, leaving 10% as a pause
    ledcWriteTone(0, melody[thisNote]);
    delay(noteDuration * 0.9);

    ledcWriteTone(0, 0);
    delay(noteDuration * 0.1);
    // stop the waveform generation before the next note.

  }

}
