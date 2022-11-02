#include "ROBLEX.h"
#include <esp_now.h>
#include <WiFi.h>

ROBLEX ROBLEX;

String RobotName = "ROBLEX Futbol 1";
#define LEFT_PWM MCPWM_UNIT_0
#define RIGHT_PWM MCPWM_UNIT_1

Adafruit_SSD1306 display(128, 64, &Wire, -1);

typedef struct struct_message {
  int leftMotor;
  int rightMotor;
  int red;
  int green;
  int blue;
} struct_message;

// Crear struct_message data
struct_message data;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  Serial.print("Bytes received: ");
  Serial.print(len);

  Serial.print("  ");
  Serial.print(data.red);
  Serial.print("  ");
  Serial.print(data.green);
  Serial.print("  ");
  Serial.print(data.blue);

  Serial.println();

  ROBLEX.RGBled(data.red, data.green, data.blue);
}

void setup() {

  ROBLEX.SetupMotor(5, LEFT_PWM, 20000);
  ROBLEX.SetupMotor(4, RIGHT_PWM, 20000);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  //for Clearing the display
  display.drawBitmap(0, 0, ROBLEX.LogoRoblex, 128, 64, WHITE);

  int namePosition = round((128 - RobotName.length() * 5) / 2);

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(namePosition, 0);
  display.print(RobotName);
  display.display();


  // ROBLEX.BtBegin(RobotName);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}


void Drive(mcpwm_unit_t unit, int out) {

  if (out < 0) {
    ROBLEX.MotorForward(unit, abs(out));

  } else if (out > 0) {
    ROBLEX.MotorBackward(unit, out);

  } else {
    ROBLEX.MotorStop(unit);
  }
}


void loop() {

  
  /*
    ROBLEX.ReadApp();

    Drive(RIGHT_PWM, ROBLEX.AppValue[0].toInt());
    Drive(LEFT_PWM, ROBLEX.AppValue[1].toInt());
    ROBLEX.RGBled(ROBLEX.AppValue[2].toInt(), ROBLEX.AppValue[3].toInt(), ROBLEX.AppValue[4].toInt());
  */
}