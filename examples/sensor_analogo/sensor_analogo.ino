#include "ROBLEX.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Ejemplo leer un modulo de sensor analogo y graficar en la pantalla oled y el debug serial
// enciende el led del modulo si el valor analogo es bajo
// es compatible con los modulos:

// Sensor de luz
// Sensor de sonido
// Potenciometro
// Botones

// estos modulos tiene el sensor en el pin A y un led en el pinB

// CONECTAR EL MODULO DE SENSOR ANALOGO EN EL PUERTO 2 Y LA PANTALLA EN CUALQUIER OTRO

#define SENSOR_PIN pin2A  // definir el sensor en el puerto 2A
#define LED_PIN pin2B     // definir el led en el puerto 2B


ROBLEX ROBLEX;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

int x[128];  //variable para la grafica

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // iniciar la pantalla oled

  ROBLEX.SetupPort(2, INPUT, OUTPUT);  // Inicia el modulo en el puerto 1

  analogReadResolution(10);
  for (int i = 127; i >= 0; i--) {
    x[i] = 10000;  // inicia los valores en un numero grande fuera de la pantalla
  }
}

void loop() {
  
  int sensor = analogRead(SENSOR_PIN);  // lee el valor del sensor analogico

  if (sensor < 300) {  // encender el led cuando el valor analogo es inferior a 300
    digitalWrite(LED_PIN, 255);
  } else {
    digitalWrite(LED_PIN, 0);  // apagar el led cuando se supere este valor
  }

  Serial.print(sensor);  // imprime el valor en el debug serial
  Serial.println();

  display.clearDisplay();  // limpia la informacion del display
  display.setTextColor(WHITE);

  // imprime la distancia en numeros en la parte superior de la pantalla
  display.setTextSize(2);    // tamano de la letra
  display.setCursor(50, 0);  // posicion del cursor en la pantalla (x,y)
  display.print(sensor);

  //escala el valor a un pixel imprimible en pantalla
  int grafica = map(sensor, 0, 1024, 63, 16); 
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