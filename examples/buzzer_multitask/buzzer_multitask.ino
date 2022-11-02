#include "COBRA.h"

Cobra Cobra;

Adafruit_SSD1306 display(128, 64, &Wire, -1);


#define BUZZZER_PIN  pin5A // ESP32 pin GIOP18 connected to piezo buzzer

int x[128]; //buffer de la grafica
int x_last[128]; //buffer secundario de la grafica


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
TaskHandle_t Task1;
void setup() {

  Cobra.SetupPort(5, OUTPUT, INPUT);
  ledcAttachPin(BUZZZER_PIN, 0);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(115200);
  Serial.println("start");
  Cobra.SetupPort(1, INPUT, INPUT);
  for (int i = 127; i >= 0; i--) {
    x[i] = 9999;
  }
  for (int i = 127; i >= 0; i--) {
    x_last[i] = 9999;
  }

  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */         /* pin task to core 0 */

}


void Task1code(void * pvParameters ) {
  Serial.println(xPortGetCoreID());
  for (;;) {
    if (digitalRead(pin5B) == LOW) {
      unsigned int n = sizeof(melody) / sizeof(melody[0]);
      Cobra.PlayMelody(melody, n, tempo);
    }
    delay(10);
  }
}

void loop() {

  int valorAnalogico = analogRead(pin1A);

  display.clearDisplay(); //limpia el buffer del display

  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(50, 0);
  display.print(valorAnalogico);


  int grafica = map(valorAnalogico, 0, 4096, 63, 15); //escala el valor analogico a un pixel imprimible en pantalla
  x[127] = grafica; //asigna el valor escalado a el ultimo dato de la matriz

  for (int i = 127; i >= 0; i--) {
    display.drawPixel(i, x[i], WHITE); //dibuja punto a punto el contenido de x
    x_last[i - 1] = x[i]; //guarda la informacion desplazada una posicion temporalmente en y
  }
  for (int i = 127; i >= 0; i--) {
    x[i] = x_last[i]; //envia los datos desplazados de vuelta a la variable x
  }

  display.display(); //despliega la informacion del buffer en la pantalla



}
