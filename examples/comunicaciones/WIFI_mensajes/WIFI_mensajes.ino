#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

//reemplazar por el nombre de su red wifi
const char* ssid = "roblex";
//reemplazar por la contrasena de su wifi
const char* password = "12345678";

//reemplazar con su numero de telefono
String phoneNumber = "+57xxxxxxxxxx";
//reemplazar con su apiKey
String apiKey = "xxxxxxx";

HTTPClient http;

void sendMessage(String message) {

  // url para HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);

  http.begin(url);

  // header de contenio
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // enviar HTTP POST request
  int httpResponseCode = http.POST(url);

  if (httpResponseCode == 200) {
    Serial.print("Mensaje enviado");
  } else {
    Serial.println("Error enviando mensaje");
    Serial.print("HTTP code: ");
    Serial.println(httpResponseCode);
  }

  // terminar conexion
  http.end();
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.println("Conectando ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a wifi con la ip: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (Serial.available()) {
    String mensaje = Serial.readString();
    sendMessage(mensaje);
  }
}