#include "ROBLEX.h"
ROBLEX ROBLEX;

#define LEFT_PWM MCPWM_UNIT_0
#define RIGHT_PWM MCPWM_UNIT_1

#define BUZZZER_PIN pin3A  // ESP32 pin connected to buzzer


Adafruit_SSD1306 display(128, 64, &Wire, -1);


String RobotName = "Futbol 2";
int namePosition = round((128 - RobotName.length() * 5) / 2);

TaskHandle_t Task1;

void setup() {

  ROBLEX.SetupMotor(5, LEFT_PWM, 20000);
  ROBLEX.SetupMotor(4, RIGHT_PWM, 20000);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  //for Clearing the display
  display.drawBitmap(0, 0, ROBLEX.LogoRoblex, 128, 64, WHITE);

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(namePosition, 0);
  display.print(RobotName);
  display.display();


  ROBLEX.BtBegin(RobotName);
  Serial.begin(115200);
  ROBLEX.SetupPort(3, OUTPUT, INPUT);

  ledcAttachPin(BUZZZER_PIN, 0);

  xTaskCreatePinnedToCore(
    Task1code,
    "Task1",
    10000,
    NULL,
    1,
    &Task1,
    0);
}


int melody[] = {

  // Super Mario Bros theme
  // Score available at https://musescore.com/user/2123/scores/2145
  // Theme by Koji Kondo
  NOTE_E5, 8, NOTE_E5, 8, REST, 8, NOTE_E5, 8, REST, 8, NOTE_C5, 8, NOTE_E5, 8,  //1
  REST, 8, NOTE_G5, 4, REST, 4

};

int tempo = 200;

void Task1code(void* pvParameters) {

  while (true) {
    if (digitalRead(pin3B) == HIGH) {
      unsigned int n = sizeof(melody) / sizeof(melody[0]);
      ROBLEX.PlayMelody(melody, n, tempo);
    }
    delay(10);
  }
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

bool penalty = false;

void loop() {


  while (!penalty) {
    ROBLEX.ReadApp();

    Drive(RIGHT_PWM, ROBLEX.AppValue[0].toInt());
    Drive(LEFT_PWM, ROBLEX.AppValue[1].toInt());
    ROBLEX.RGBled(ROBLEX.AppValue[2].toInt(), ROBLEX.AppValue[3].toInt(), ROBLEX.AppValue[4].toInt());

    if (analogRead(boton) > 1000) {
      penalty = true;
    }
  }

  //penalty


  Drive(RIGHT_PWM, 30);
  Drive(LEFT_PWM, -28);
  delay(10000);
  Drive(RIGHT_PWM, 0);
  Drive(LEFT_PWM, 0);
  penalty = false;
}