//control C8:F0:9E:A6:2B:9C
//robot futbol 1 C8:F0:9E:A0:E3:DC
#include "ROBLEX.h"
#include <esp_now.h>
#include <WiFi.h>

ROBLEX ROBLEX;

// Remplazar con el address del receptor
uint8_t robotAddress[] = { 0xC8, 0xF0, 0x9E, 0xA0, 0xE3, 0xDC };

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int leftMotor;
  int rightMotor;
  int red;
  int green;
  int blue;
} struct_message;

// Crear struct_message data
struct_message data;

esp_now_peer_info_t peerInfo;

// funcion cuando se envian datos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void setup() {
  ROBLEX.SetupPort(4, INPUT, INPUT);
  ROBLEX.SetupPort(5, INPUT, INPUT);
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, robotAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {

  int joy = analogRead(pin4A);
  Serial.print(joy);
  Serial.print("  ");
  Serial.print(analogRead(pin4B));
  Serial.print("      ");
  Serial.print(analogRead(pin5A));
  Serial.print("  ");
  Serial.print(analogRead(pin5B));
  Serial.println();

  // Set values to send
  data.red = 100;
  data.green = 100;
  data.blue = 100;
  /*
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(robotAddress, (uint8_t *)&data, sizeof(data));

  ROBLEX.RGBled(data.red, data.green, data.blue);
*/
  delay(50);
}