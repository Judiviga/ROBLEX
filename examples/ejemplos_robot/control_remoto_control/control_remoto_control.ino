#include "ROBLEX.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
ROBLEX ROBLEX;

// Ejemplo de control remoto fisico (otro ESP32 + placa ROBLEX con joysticks)
// que se conecta al robot por BLE y le envia los comandos.

// MAC unica del robot al que se conecta este control. La imprime el robot por
// el monitor serial al encender ("Direccion BLE del robot: ..."). Conectar por
// MAC evita ambiguedades cuando hay varios robots con el mismo nombre.
String robotAddress = "D4:D4:DA:E4:A2:DE";

String ControlName = "Control ROBLEX";

Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define JOY_LX pin4A
#define JOY_LY pin4B

#define JOY_RX pin5A
#define JOY_RY pin5B

#define GATILLO_L pin2A
#define GATILLO_R pin2B

#define POT pin1A
#define POT_LED pin1B

#define BTNS pin3A
#define BTNS_LED pin3B

void setup() {

  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  //for Clearing the display
  display.drawBitmap(0, 0, ROBLEX.LogoRoblex, 128, 64, WHITE);
  int namePosition = round((128 - ControlName.length() * 5) / 2);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(namePosition, 0);
  display.print(ControlName);
  display.display();

  pinMode(JOY_LY, INPUT);
  pinMode(JOY_LX, INPUT);

  pinMode(JOY_RY, INPUT);
  pinMode(JOY_RX, INPUT);

  pinMode(GATILLO_L, INPUT);
  pinMode(GATILLO_R, INPUT);

  pinMode(POT, INPUT);
  pinMode(POT_LED, OUTPUT);

  pinMode(BTNS, INPUT);
  pinMode(BTNS_LED, OUTPUT);

  // Conectarse al robot por su MAC unica (reintenta hasta lograrlo)
  while (!ROBLEX.BluetoothConnectAddress(robotAddress)) {
    Serial.println("No se encontro el robot. Verifica que este encendido y en rango.");
  }
  Serial.println("Conectado al robot!");
  ROBLEX.Rgb(100, 100, 100);
}

int r, g, b;

float giroVel = 0.5;

void loop() {

  //seleccionar color del RGB con el potenciometro
  int value = map(analogRead(POT), 0, 4095, 0, 250);
  ColorPicker(value);

  //leer los joysticks
  int y = analogRead(JOY_LY);
  int x = analogRead(JOY_RX);

  //configurar la maxima velocidad segun potenciometro
  int max = map(analogRead(POT), 0, 4095, 40, 100);

  y = map(y, 0, 4095, -max, max);
  x = map(x, 0, 4095, -max * giroVel, max * giroVel);

  //velocidad minima
  if (abs(y) < 20) y = 0;
  if (abs(x) < 5) x = 0;


  if (digitalRead(GATILLO_L) == HIGH) {
    y = -255;
  } else if (digitalRead(GATILLO_R) == HIGH) {
    y = 255;
  }

  int outL = (y + x);
  int outR = -(y - x);

  String message = String(-outL) + "," + String(-outR) + "," + String(r) + "," + String(g) + "," + String(b);


  if (ROBLEX.BluetoothRobotConnected()) {
    ROBLEX.BluetoothSend(message);
  } else {
    //si se perdio la conexion, reintentar
    ROBLEX.BluetoothConnectAddress(robotAddress);
  }
  Serial.println(message);

  delay(50);
}




void ColorPicker(int value) {

  if (value <= 100) {
    r = 100 - value;
    g = value;
    b = 0;
  } else if (value <= 200) {
    r = 0;
    g = 200 - value;
    b = value - 100;
  } else if (value <= 250) {
    r = value - 200;
    g = 0;
    b = 300 - value;
  }
  ROBLEX.Rgb(r, g, b);
}
