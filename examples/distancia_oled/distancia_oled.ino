#include "ROBLEX.h"

// Ejemplo leer el modulo de distacia y imprimir grafica con el modulo Pantalla oled y el debug serial

// CONECTAR EL MODULO DE DISTANCIA EN EL PUERTO 2 Y EL MODULO PANTALLA OLED EN CUALQUIER OTRO PUERTO

ROBLEX ROBLEX;
Adafruit_SSD1306 display(128, 64, &Wire, -1);  // configurar la pantalla oled

int x[128]; //variable para la grafica

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // iniciar la pantalla oled

  ROBLEX.SetupDistance(2, false, true, false);  //iniciar el modulo de distancia en el puerto 2 con el sensor frontal

  /*
para usar el sensor izquierdo:
  ROBLEX.SetupDistance(2, true, false, false);

para usar el sensor frontal:
  ROBLEX.SetupDistance(2, false, true, false);

para usar el sensor derecho:
  ROBLEX.SetupDistance(2, false, false, true);

puede usar varios sensores simultaneamente cambiando a true respectivamente
*/

  for (int i = 127; i >= 0; i--) {
    x[i] = 10000; // inicia los valores en un numero grande fuera de la pantalla
  }
}

void loop() {

  int distancia = ROBLEX.FrontRange();  // leer el sensor de distancia frontal

  Serial.print(distancia); // imprime la distancia en el debug serial
  Serial.println();

  display.clearDisplay();  //limpia la informacion del display
  display.setTextColor(WHITE);

  // imprime la distancia en numeros en la parte superior de la pantalla
  display.setTextSize(2);    // tamano de la letra
  display.setCursor(50, 0);  // posicion del cursor en la pantalla (x,y)
  display.print(distancia);

  int grafica = map(distancia, 0, 800, 63, 15);  //escala la distancia a un pixel imprimible en la pantalla

  x[127] = grafica;  //asigna el valor de distancia a el ultimo dato de la serie

  for (int i = 127; i >= 0; i--) {
    display.drawPixel(i, x[i], WHITE);  //dibuja punto a punto el contenido de la serie
  }
  for (int i = 0; i < 127; i++) {
    x[i] = x[i + 1];  //desplaza los valores un punto a la izquierda
  }

  display.display();  // imprime la informacion en la pantalla
}