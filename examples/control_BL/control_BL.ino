#include "ROBLEX.h"
#include "BluetoothSerial.h"
ROBLEX ROBLEX;
// Remplazar con el address del receptor

//uint8_t robotAddress[] = { 0xC8, 0xF0, 0x9E, 0xA0, 0xE3, 0xDE };//1
//uint8_t robotAddress[] = { 0x7C, 0x9E, 0xBD, 0x48, 0x4D, 0x1A }; //2
uint8_t robotAddress[] = { 0xC8, 0xF0, 0x9E, 0x9C, 0x24, 0xB6 };

String ControlName = "ROBLEX Control 3";

BluetoothSerial BT;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {

  BT.begin(ControlName, true);
  Serial.begin(115200);
  bool connected = BT.connect(robotAddress);
  Serial.println("START");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  //for Clearing the display
  display.drawBitmap(0, 0, ROBLEX.LogoRoblex, 128, 64, WHITE);
  int namePosition = round((128 - ControlName.length() * 5) / 2);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(namePosition, 0);
  display.print(ControlName);
  display.display();

  ROBLEX.SetupPort(2, INPUT, INPUT);
  ROBLEX.SetupPort(3, INPUT, OUTPUT);
  ROBLEX.SetupPort(4, INPUT, INPUT);
  ROBLEX.SetupPort(5, INPUT, INPUT);

  if (connected) {
    Serial.println("Connected Succesfully!");
    ROBLEX.RGBled(100, 100, 100);
  } else {
    while (!BT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
    }
  }
}
int r, g, b;

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
  ROBLEX.RGBled(r, g, b);
}


void loop() {

  int value = map(analogRead(pin3A), 0, 4095, 0, 250);
  ColorPicker(value);

  int y = analogRead(pin4B);
  int x = analogRead(pin5A);

  int max = map(analogRead(pin3A), 0, 4095, 40, 100);


  y = map(y, 0, 4095, -max, max);
  x = map(x, 0, 4095, -max * 0.5, max * 0.5);

  if (abs(y) < 20) y = 0;
  if (abs(x) < 5) x = 0;


  if (digitalRead(pin2A) == HIGH) {
    y = -255;
  } else if (digitalRead(pin2B) == HIGH) {
    y = 255;
  }

  int outL = -(y + x);
  int outR = (y - x);
  /*
  Serial.print(outL);
  Serial.print("  ");
  Serial.print(outR);
  Serial.println();
*/
  String message = String(outL) + "," + String(outR) + "," + String(r) + "," + String(g) + "," + String(b);
  if (BT.connected()) {
    BT.println(message);
  } else {
    BT.connect(robotAddress);
  }
  Serial.println(message);

  delay(50);
}