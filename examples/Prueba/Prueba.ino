#include "ROBLEX.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

ROBLEX ROBLEX;  // llamar libreria ROBLEX

// Ejemplo para probar el funcionamiento de los modulos

// MODULO SENSOR ANALOGO ( POTENCIOMETRO, SENSOR DE LUZ, SENSOR DE SONIDO) EN EL PUERTO 1
// MODULO SENSOR DE DISTANCIA EN EL PUERTO 2
// MODULO BUZZER EN EL PUERTO 3
// MODULO PANTALLA OLED EN EL PUERTO 4
// MODULO BOTONES EN EL PUERTO 5

Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define BOTONES_PIN pin5A  // definir el sensor en el puerto 2A
#define BTNLED_PIN pin5B   // definir el led en el puerto 2B

#define SENSOR_PIN pin1A  // definir el sensor en el puerto 2A
#define SNSLED_PIN pin1B    // definir el led en el puerto 2B

#define BUZZZER_PIN pin3A  // definir el buzzer en el puerto 3A
#define BOTON_PIN pin3B    // definir el boton en el puerto 3B
#define BUZZER_CHANNEL 0   // definir canal de salida del pwm


int x[128];  //variable para la grafica

int melody[] = {
  // Super Mario Bros theme
  // Disponible en https://github.com/robsoncouto/arduino-songs/blob/master/supermariobros/supermariobros.ino
  // Theme by Koji Kondo
  NOTE_E5, 8, NOTE_E5, 8, REST, 8, NOTE_E5, 8, REST, 8, NOTE_C5, 8, NOTE_E5, 8,
  REST, 8, NOTE_G5, 4
};

int tempo = 200;  //velocidad de reproduccion de la melodia tomada de la misma pagina

TaskHandle_t Task1;  // crear tarea adicional

int distanciaMM = 0;

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  pinMode(SENSOR_PIN, INPUT);                  //define el pin A como entrada
  pinMode(SNSLED_PIN, OUTPUT);                   //define el pin B como salida
  pinMode(BOTONES_PIN, INPUT);                 //define el pin A como entrada
  pinMode(BTNLED_PIN, OUTPUT);                 //define el pin B como salida
  pinMode(BOTON_PIN, INPUT);                   // configurar el boton como entrada
  ledcAttachPin(BUZZZER_PIN, BUZZER_CHANNEL);  // configurar el buzzer como salida PWM en el canal 0

  ROBLEX.SetupDistance(2, false, true, false);  //iniciar el modulo de distancia en el puerto 2 con el sensor frontal

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // iniciar la pantalla oled

  for (int i = 127; i >= 0; i--) {
    x[i] = 10000;  // inicia los valores en un numero grande fuera de la pantalla
  }

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
    if (digitalRead(BOTON_PIN) == HIGH) {                   // reproducir cuando se presiona el boton del modulo
      int n = sizeof(melody) / sizeof(melody[0]);           // calcular el tamano de la melodia
      ROBLEX.PlayMelody(BUZZER_CHANNEL, melody, n, tempo);  // reproducir la melodia (melodia, tamano de la melodia, velocidad)
    }
    delay(20);
  }
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
  } else if (valor > 2000) {
    return ABAJO;
  } else if (valor > 1000) {
    return IZQUIERDA;
  } else {
    return NINGUNO;
  }
}

void loop() {

  if (digitalRead(boton) == 1) {
    digitalWrite(SNSLED_PIN, HIGH);
  } else {
    digitalWrite(SNSLED_PIN, LOW);
  }

  // obtiene el boton presionado haciendo uso de la funcion creada anteriormente
  int presionado = botones(analogRead(BOTONES_PIN));
  // realizar funciones segun el boton presionado
  if (presionado == ARRIBA) {  // encender el led del modulo de botones si se presiona el boton de arriba
    digitalWrite(BTNLED_PIN, 255);
  } else {
    digitalWrite(BTNLED_PIN, 0);
  }
  if (presionado == DERECHA) {  // cambiar el color del led rgb con los otros botones
    ROBLEX.Rgb(255, 0, 0);      //rojo
  } else if (presionado == ABAJO) {
    ROBLEX.Rgb(0, 255, 0);  //verde
  } else if (presionado == IZQUIERDA) {
    ROBLEX.Rgb(0, 0, 255);  //azul
  } else {
    ROBLEX.Rgb(0, 0, 0);
  }
  if (presionado == NINGUNO) {
  }


  Serial.println(analogRead(pin1A));

  if (ROBLEX.FrontComplete()) {
    distanciaMM = ROBLEX.FrontRange();
  }
  int sensor = distanciaMM;  // lee el valor del sensor analogico

  display.clearDisplay();  // limpia la informacion del display
  display.setTextColor(WHITE);

  // imprime la distancia en numeros en la parte superior de la pantalla
  display.setTextSize(2);    // tamano de la letra
  display.setCursor(50, 0);  // posicion del cursor en la pantalla (x,y)
  display.print(sensor);

  //escala el valor a un pixel imprimible en pantalla
  int grafica = map(sensor, 00, 800, 63, 16);
  // map(valor minimo del sensor, valor maximo del sensor, punto inferior de la pantalla, punto superior de la pantalla)

  x[127] = grafica;  //asigna el valor escalado a el ultimo dato de la matriz

  for (int i = 127; i >= 0; i--) {
    display.drawPixel(i, x[i], WHITE);  //dibuja punto a punto el contenido de la serie
  }
  for (int i = 0; i < 127; i++) {
    x[i] = x[i + 1];  //desplaza los valores un punto a la izquierda
  }
  display.display();  // imprime la informacion en la pantalla
  delay(10);
}
