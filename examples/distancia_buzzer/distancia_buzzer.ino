#include "COBRA.h"

Cobra Cobra;

Adafruit_SSD1306 display(128, 64, &Wire, -1);

int x[128]; //buffer de la grafica
int x_last[128]; //buffer secundario de la grafica

void setup() {
  digitalWrite(pin5A, HIGH);

  Serial.begin(115200);

  Serial.println("start");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Cobra.BtBegin("COBRA 5");

  Cobra.SetupPort(5, OUTPUT, INPUT);
  Cobra.SetupDistance(2, false, true, false);

  for (int i = 127; i >= 0; i--) {
    x[i] = 9999;
  }
  for (int i = 127; i >= 0; i--) {
    x_last[i] = 9999;
  }

}


void loop() {


  int  distancia = Cobra.FrontRange();

  if (distancia < 500) {
    Cobra.RGBled(100, 0, 0);
    digitalWrite(pin5A, LOW);
  }
  else {
    Cobra.RGBled(0, 0, 0);
    digitalWrite(pin5A, HIGH);
  }


  Serial.print(distancia);
  Serial.println();

  display.clearDisplay(); //limpia el buffer del display
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(50, 0);
  display.print(distancia);

  int grafica = map(distancia, 0, 800, 63, 15); //escala el valor analogico a un pixel imprimible en pantalla

  x[127] = grafica; //asigna el valor escalado a el ultimo dato de la matriz

  for (int i = 127; i >= 0; i--) {
    display.drawPixel(i, x[i], WHITE); //dibuja punto a punto el contenido de x
    x_last[i - 1] = x[i]; //guarda la informacion desplazada una posicion temporalmente en y
  }
  for (int i = 127; i >= 0; i--) {
    x[i] = x_last[i]; //envia los datos desplazados de vuelta a la variable x
  }

  display.display(); //despliega la informacion del buffer en la

}
