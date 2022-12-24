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


  pinMode(SENSOR_PIN, INPUT);  //define el pin A como entrada
  pinMode(LED_PIN, OUTPUT);    //define el pin B como salida

  analogReadResolution(10);

  for (int i = 127; i >= 0; i--) {
    x[i] = 10000;  // inicia los valores en un numero grande fuera de la pantalla
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

  if (valor > 1010) {
    return ARRIBA;
  } else if (valor > 700) {
    return DERECHA;
  } else if (valor > 300) {
    return ABAJO;
  } else if (valor > 180) {
    return IZQUIERDA;
  } else {
    return NINGUNO;
  }
}

void loop() {

  int valor = analogRead(SENSOR_PIN);  // lee el valor del sensor analogico

  // obtiene el boton presionado haciendo uso de la funcion creada anteriormente
  int presionado = botones(valor);

  // realizar funciones segun el boton presionado
  if (presionado == ARRIBA) {  // encender el led del modulo de botones si se presiona el boton de arriba
    digitalWrite(LED_PIN, 255);
    Serial.print("ARRIBA");
  } else {
    digitalWrite(LED_PIN, 0);
  }

  if (presionado == DERECHA) {  // cambiar el color del led rgb con los otros botones
    ROBLEX.Rgb(255, 0, 0);      //rojo
    Serial.print("DERECHA");
  } else if (presionado == ABAJO) {
    ROBLEX.Rgb(0, 255, 0);  //verde
    Serial.print("ABAJO");
  } else if (presionado == IZQUIERDA) {
    ROBLEX.Rgb(0, 0, 255);  //azul
    Serial.print("IZQUIERDA");
  } else {
    ROBLEX.Rgb(0, 0, 0);
  }
  if (presionado == NINGUNO) {
    Serial.print("NINGUNO");
  }

  Serial.println();



  display.clearDisplay();  // limpia la informacion del display
  display.setTextColor(WHITE);

  // imprime la distancia en numeros en la parte superior de la pantalla
  display.setTextSize(2);    // tamano de la letra
  display.setCursor(50, 0);  // posicion del cursor en la pantalla (x,y)
  display.print(valor);

  //escala el valor a un pixel imprimible en pantalla
  int grafica = map(valor, 0, 1024, 63, 16);
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