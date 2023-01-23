#include "ROBLEX.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Ejemplo leer el modulo sensor de sonido
// enciende el led del modulo si el valor analogo es bajo

// CONECTAR EL MODULO DE SENSOR ANALOGO EN EL PUERTO 2 Y LA PANTALLA EN CUALQUIER OTRO

#define SENSOR_PIN pin1A  // definir el sensor en el puerto 2A
#define LED_PIN pin1B     // definir el led en el puerto 2B


ROBLEX ROBLEX;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

int x[128];  //variable para la grafica

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // iniciar la pantalla oled

  pinMode(SENSOR_PIN, INPUT);  //define el pin A como entrada
  pinMode(LED_PIN, OUTPUT);    //define el pin B como salida

  for (int i = 127; i >= 0; i--) {
    x[i] = 10000;  // inicia los valores en un numero grande fuera de la pantalla
  }
}

void graficar(int valor, int min, int max) {

  display.clearDisplay();  // limpia la informacion del display
  display.setTextColor(WHITE);

  // imprime la distancia en numeros en la parte superior de la pantalla
  display.setTextSize(2);    // tamano de la letra
  display.setCursor(50, 0);  // posicion del cursor en la pantalla (x,y)
  display.print(valor);

  //escala el valor a un pixel imprimible en pantalla
  int grafica = map(valor, min, max, 63, 16);
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


void loop() {

  int sensor = analogRead(SENSOR_PIN);  // lee el valor del sensor analogico

  if (sensor < 1500) {  // encender el led cuando el valor analogo es inferior a 300
    digitalWrite(LED_PIN, 255);
  } else {
    digitalWrite(LED_PIN, 0);  // apagar el led cuando se supere este valor
  }

  Serial.print(sensor);  // imprime el valor en el debug serial
  Serial.println();

  graficar(sensor, 800, 2000); //grafica en la pantalla oled el valor del sensor

}