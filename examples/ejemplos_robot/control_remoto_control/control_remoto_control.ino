#include "ROBLEX.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
ROBLEX ROBLEX;

// Ejemplo de control remoto fisico (otro ESP32 + placa ROBLEX con joysticks)
// que se conecta al robot por BLE y le envia los comandos.
//
// BUSCADOR DE ROBOTS: al encender, si no hay un robot guardado (o si mantienes
// presionado GATILLO_L mientras enciende), aparece en la OLED una lista de los
// robots encontrados. Navega con el joystick izquierdo (arriba/abajo) y
// selecciona con GATILLO_R. El robot elegido se guarda y la proxima vez se
// conecta solo, sin tener que tocar el codigo.

String ControlName = "Control ROBLEX";

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Preferences prefs;

String robotAddress = "";  // MAC del robot pareado (se guarda en memoria)
String robotName = "";     // nombre del robot pareado (para mostrar en pantalla)

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

#define BOTON boton  // boton de a bordo de la shield (GPIO36), activo en ALTO

// Dibuja la lista de robots en la OLED (con scroll si hay muchos)
void drawRobotMenu(int sel, int n) {
  const int VISIBLE = 5;
  int first = 0;
  if (sel >= VISIBLE) first = sel - VISIBLE + 1;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Elige robot:");

  for (int i = first; i < n && i < first + VISIBLE; i++) {
    display.setCursor(0, 16 + (i - first) * 10);
    display.print(i == sel ? "> " : "  ");
    display.println(ROBLEX.BluetoothScanName(i));
  }
  display.display();
}

// Escanea y deja elegir un robot; devuelve su MAC.
String selectRobot() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Buscando robots...");
  display.display();

  int n = ROBLEX.BluetoothScan(4);

  if (n == 0) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Sin robots.");
    display.println("");
    display.println("BOTON: reintentar");
    display.display();
    while (digitalRead(BOTON) != HIGH) delay(50);
    delay(300);
    return selectRobot();
  }

  int sel = 0;
  while (true) {
    drawRobotMenu(sel, n);

    int jy = analogRead(JOY_LY);
    if (jy > 3000) {  // joystick arriba
      sel = (sel - 1 + n) % n;
      delay(200);
    } else if (jy < 1000) {  // joystick abajo
      sel = (sel + 1) % n;
      delay(200);
    }

    if (digitalRead(BOTON) == HIGH) {  // seleccionar con el boton
      delay(300);
      robotName = ROBLEX.BluetoothScanName(sel);  // recordar el nombre elegido
      return ROBLEX.BluetoothScanAddress(sel);
    }
    delay(40);
  }
}

void setup() {

  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  //for Clearing the display
  display.setTextColor(WHITE);
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

  pinMode(BOTON, INPUT);  // boton de a bordo para seleccionar / resetear

  // Cargar el robot guardado (nombre + MAC)
  prefs.begin("roblex", false);
  robotAddress = prefs.getString("robot", "");
  robotName = prefs.getString("robotName", "");

  // RESETEAR / CAMBIAR DE ROBOT: abrir el buscador si no hay robot guardado, o
  // si se mantiene el BOTON presionado al encender el control.
  if (robotAddress == "" || digitalRead(BOTON) == HIGH) {
    robotAddress = selectRobot();             // tambien fija robotName
    prefs.putString("robot", robotAddress);   // recordar la eleccion
    prefs.putString("robotName", robotName);
  }

  // Conectarse al robot elegido (reintenta hasta lograrlo)
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Conectando a:");
  display.setCursor(0, 16);
  display.setTextSize(1);
  display.println(robotName);  // mostrar el NOMBRE, no la MAC
  display.display();

  while (!ROBLEX.BluetoothConnectAddress(robotAddress)) {
    Serial.println("No se encontro el robot. Verifica que este encendido y en rango.");
  }
  Serial.println("Conectado al robot!");

  // Pantalla de conectado con el nombre del robot
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Conectado a:");
  display.setTextSize(1);
  display.setCursor(0, 16);
  display.println(robotName);
  display.display();

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
