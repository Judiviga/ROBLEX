#include "ROBLEX.h"
#include "Adafruit_TCS34725.h"
ROBLEX ROBLEX;

#define LEFT_PWM   MCPWM_UNIT_0
#define RIGHT_PWM   MCPWM_UNIT_1


Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int x[128]; //buffer de la grafica
int x_last[128]; //buffer secundario de la grafica

#define BUZZZER_PIN  pin3A 
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

  ROBLEX.SetupMotor(5, LEFT_PWM, 20000);
  ROBLEX.SetupMotor(4, RIGHT_PWM, 20000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  ROBLEX.BtBegin("ROBLEX ROBOT");
  Serial.begin(115200);
  ROBLEX.SetupPort(3, OUTPUT, INPUT);
  ROBLEX.SetupDistance(2);


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
    if (digitalRead(pin3B) == HIGH) {
      unsigned int n = sizeof(melody) / sizeof(melody[0]);
      ROBLEX.PlayMelody(melody, n, tempo);
    }
    delay(10);
  }
}


void Drive(mcpwm_unit_t unit, int out) {

  if (out < 0) {
    ROBLEX.MotorForward(unit, abs(out));

  } else if (out > 0) {
    ROBLEX.MotorBackward(unit, out);

  } else {
    ROBLEX.MotorStop(unit);
  }
}

int distanciaMM = 0;
int scale = 100;


void loop() {

  ROBLEX.ReadApp();

  Drive(RIGHT_PWM,  ROBLEX.AppValue[0].toInt());
  Drive(LEFT_PWM, ROBLEX.AppValue[1].toInt());
  ROBLEX.RGBled(ROBLEX.AppValue[2].toInt(), ROBLEX.AppValue[3].toInt(), ROBLEX.AppValue[4].toInt());

  if (ROBLEX.FrontComplete()) {
    distanciaMM =  ROBLEX.FrontRange();
  }


  float  distanciaCM = distanciaMM / 10;


  display.clearDisplay(); //limpia el buffer del display
  int grafica = map(distanciaCM, 0, scale, 63, 15); //escala el valor analogico a un pixel imprimible en pantalla
  x[127] = grafica; //asigna el valor escalado a el ultimo dato de la matriz
  for (int i = 127; i >= 0; i--) {
    display.drawPixel(i, x[i], WHITE); //dibuja punto a punto el contenido de x
    x_last[i - 1] = x[i]; //guarda la informacion desplazada una posicion temporalmente en y
  }
  for (int i = 127; i >= 0; i--) {
    x[i] = x_last[i]; //envia los datos desplazados de vuelta a la variable x
  }

  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(40, 0);
  if (distanciaCM >= 819) {
    display.print("---");
  } else {
    display.print(int(distanciaCM));
  }
  display.print(" cm");
  display.display(); //despliega la informacion del buffer en la pantalla



}
