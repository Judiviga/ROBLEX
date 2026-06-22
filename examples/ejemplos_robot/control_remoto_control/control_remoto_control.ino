#include "ROBLEX.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
ROBLEX ROBLEX;

// Ejemplo de control remoto fisico (otro ESP32 + placa ROBLEX con joysticks)
// que se conecta al robot por BLE y le envia los comandos.
//
// BUSCADOR DE ROBOTS: al encender, si no hay un robot guardado (o si mantienes
// presionado el BOTON de la shield mientras enciende), aparece en la OLED una
// lista de los robots encontrados. Navega con el joystick izquierdo
// (arriba/abajo) y selecciona con el BOTON. El robot elegido se guarda y la
// proxima vez se conecta solo, sin tener que tocar el codigo.

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Preferences prefs;

String robotAddress = "";  // MAC del robot pareado (se guarda en memoria)
String robotName = "";     // nombre del robot pareado (para mostrar en pantalla)

int r, g, b;
float giroVel = 0.5;

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

void setup() {

  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
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
  pinMode(BOTON, INPUT);  // boton para seleccionar / resetear

  // Cargar el robot guardado (nombre + MAC)
  prefs.begin("roblex", false);
  robotAddress = prefs.getString("robot", "");
  robotName = prefs.getString("robotName", "");

  // RESETEAR / CAMBIAR DE ROBOT: abrir el buscador si no hay robot guardado, o
  // si se mantiene el BOTON presionado al encender el control.
  if (robotAddress == "" || digitalRead(BOTON) == HIGH) {
    robotAddress = ROBLEX.SelectRobot(display, BOTON, JOY_LY);  // menu en la OLED
    robotName = ROBLEX.SelectedRobotName();
    prefs.putString("robot", robotAddress);
    prefs.putString("robotName", robotName);
  }

  // Conectarse al robot elegido (reintenta hasta lograrlo)
  while (!ROBLEX.BluetoothConnectAddress(robotAddress)) {
    Serial.println("No se encontro el robot. Verifica que este encendido y en rango.");
  }
  Serial.println("Conectado al robot!");

  ROBLEX.ShowStatus(display, true, robotName);
  ROBLEX.Rgb(100, 100, 100);
}

void loop() {

  //seleccionar color del RGB con el potenciometro
  int value = map(analogRead(POT), 0, 4095, 0, 250);
  ROBLEX.ColorWheel(value, r, g, b);  // setea r,g,b y enciende el LED

  //leer los joysticks
  int y = analogRead(JOY_LY);
  int x = analogRead(JOY_RX);

  //configurar la maxima velocidad segun potenciometro
  int max = map(analogRead(POT), 0, 4095, 40, 100);

  y = map(y, 0, 4095, -max, max);
  x = map(x, 0, 4095, -max * giroVel, max * giroVel);

  //velocidad minima
  if (abs(y) < 20) y = 0;
  if (abs(x) < 10) x = 0;

  if (digitalRead(GATILLO_L) == HIGH) {
    y = -255;
  } else if (digitalRead(GATILLO_R) == HIGH) {
    y = 255;
  }

  int outL = (y + x);
  int outR = -(y - x);

  String message = String(-outL) + "," + String(-outR) + "," + String(r) + "," + String(g) + "," + String(b);

  // Detectar cambios de conexion y mostrarlos en la pantalla
  bool connected = ROBLEX.BluetoothRobotConnected();
  static bool lastConnected = true;  // en setup ya quedo conectado
  if (connected != lastConnected) {
    ROBLEX.ShowStatus(display, connected, robotName);
    lastConnected = connected;
  }

  if (connected) {
    ROBLEX.BluetoothSend(message);
  } else {
    //si se perdio la conexion, reintentar
    ROBLEX.BluetoothConnectAddress(robotAddress);
  }
  Serial.println(message);

  delay(50);
}
