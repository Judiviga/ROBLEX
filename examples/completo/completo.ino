#include "COBRA.h"
#include "Adafruit_TCS34725.h"

Cobra Cobra;
Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int x[128]; //buffer de la grafica
int x_last[128]; //buffer secundario de la grafica

#define BUZZZER_PIN  pin3A // ESP32 pin GIOP18 connected to piezo buzzer
int melody[] = {

  // Super Mario Bros theme
  // Score available at https://musescore.com/user/2123/scores/2145
  // Theme by Koji Kondo
  NOTE_E5, 8, NOTE_E5, 8, REST, 8, NOTE_E5, 8, REST, 8, NOTE_C5, 8, NOTE_E5, 8, //1
  REST, 8, NOTE_G5, 4, REST, 4

};

int  tempo = 200;
TaskHandle_t Task1;


void clearGraph() {
  for (int i = 127; i >= 0; i--) {
    x[i] = 9999;
  }
  for (int i = 127; i >= 0; i--) {
    x_last[i] = 9999;
  }
}


void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(115200);
  Serial.println("start");
  Cobra.SetupPort(4, INPUT, INPUT);
  Cobra.SetupPort(3, OUTPUT, INPUT);
  Cobra.SetupPort(1, INPUT, OUTPUT);
  Cobra.SetupDistance(5);
  //tcs.begin();
  ledcAttachPin(BUZZZER_PIN, 0);


  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */         /* pin task to core 0 */

  clearGraph();
  tcs.begin();
}
void Task1code(void * pvParameters ) {

  while (true) {
    if (digitalRead(pin3B) == LOW) {
      unsigned int n = sizeof(melody) / sizeof(melody[0]);
      Cobra.PlayMelody(melody, n, tempo);
    }
    delay(10);
  }
}


int show = 0;
int valorAnalogico = 0;
int scale = 4096;

int color = 0;
int distancia;

void loop() {

  int selector = analogRead(pin4A);


  if (selector < 100) {
    show = 2;
  } else if (selector < 1000) {
    show = 1;
    clearGraph();
  } else if (selector < 2500) {

  } else if (selector < 3500) {
    show = 0;
    clearGraph();
  } else {

  }


  if (analogRead(pin1A) < 500) {
    digitalWrite(pin1B, HIGH);
  } else {
    digitalWrite(pin1B, LOW);
  }


  if (show == 0) {
    valorAnalogico = analogRead(pin1A);
    scale = 4096;
  } else if (show == 1) {
    distancia = Cobra.FrontRange();
    valorAnalogico = distancia;
    scale = 800;
  } else if (show == 2) {
    float red, green, blue;
    tcs.getRGB(&red, &green, &blue);
    Cobra.RGBled(int(red), int(green), int(blue));
  }


  //if (show != 2) {
  display.clearDisplay(); //limpia el buffer del display

  int grafica = map(valorAnalogico, 0, scale, 63, 15); //escala el valor analogico a un pixel imprimible en pantalla
  x[127] = grafica; //asigna el valor escalado a el ultimo dato de la matriz

  for (int i = 127; i >= 0; i--) {
    display.drawPixel(i, x[i], WHITE); //dibuja punto a punto el contenido de x
    x_last[i - 1] = x[i]; //guarda la informacion desplazada una posicion temporalmente en y
  }
  for (int i = 127; i >= 0; i--) {
    x[i] = x_last[i]; //envia los datos desplazados de vuelta a la variable x
  }

  Serial.print(valorAnalogico);
  Serial.println();

  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(50, 0);
  display.print(valorAnalogico);
  display.display(); //despliega la informacion del buffer en la pantalla
  // }
}
