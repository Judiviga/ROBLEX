#include "ROBLEX.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Ejemplo terminal BLE con la app ROBLEX (seccion "Terminal")
//   - el texto que se escribe en la terminal de la app se muestra en la pantalla oled
//   - cada segundo el robot envia a la app el estado del boton de la shield:
//       "BOTON,1"  presionado
//       "BOTON,0"  suelto
// Conectarse al robot desde la app, abrir la Terminal y escribir un mensaje.

// CONECTAR LA PANTALLA OLED AL BUS I2C DE LA SHIELD

#define PERIODO_BOTON 1000  // ms entre envios del estado del boton

ROBLEX ROBLEX;
Adafruit_SSD1306 display(128, 64, &Wire, -1);  // configurar la pantalla oled

String RobotName = "ROBLEX terminal";  // nombre del robot en el bluetooth

unsigned long ultimoEnvio = 0;



// muestra un mensaje en la pantalla
void mostrarMensaje(String texto) {

  display.clearDisplay();  // limpia la informacion del display

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Terminal BLE");
  display.drawLine(0, 10, 127, 10, WHITE);  // linea debajo del titulo

  // los mensajes cortos con letra grande; los largos con letra pequena para que quepan
  if (texto.length() <= 30) {
    display.setTextSize(2);  // 10 letras por linea, 3 lineas
  } else {
    display.setTextSize(1);  // 21 letras por linea, 6 lineas
  }
  display.setCursor(0, 16);
  display.print(texto);  // pasa a la siguiente linea al llegar al borde

  display.display();  // imprime la informacion en la pantalla
}

void setup() {

  Serial.begin(115200);  // iniciar debug serial

  pinMode(boton, INPUT);  // boton de la shield (activo en ALTO)

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // iniciar la pantalla oled
  display.setTextColor(WHITE);
  mostrarMensaje("Esperando mensaje...");

  ROBLEX.BluetoothBegin(RobotName);  //iniciar la comunicacion BLE con el nombre asignado
  Serial.print("Direccion BLE: ");
  Serial.println(ROBLEX.BluetoothAddress());
}

void loop() {

  // mostrar en la pantalla cada mensaje que llega de la app
  if (ROBLEX.BluetoothAvailable()) {
    String texto = ROBLEX.BluetoothRead();  // la linea que se escribio en la terminal, sin el salto de linea
    Serial.println("App: " + texto);
    mostrarMensaje(texto);
  }

  // cada segundo enviar a la app el estado del boton de la shield
  if (millis() - ultimoEnvio >= PERIODO_BOTON) {
    ultimoEnvio = millis();

    int estado = digitalRead(boton);  // 1 = presionado, 0 = suelto
    String mensaje = String(estado);
    ROBLEX.BluetoothWrite(mensaje);  // se ve en la terminal de la app (si esta conectada)
    Serial.println(mensaje);
  }
}
