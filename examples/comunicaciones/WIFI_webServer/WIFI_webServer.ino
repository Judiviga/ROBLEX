#include "ROBLEX.h"
#include <WiFi.h>

ROBLEX ROBLEX;

// Replace with your network credentials
const char* ssid = "roblex";
const char* password = "12345678";

// Set web server port number to 80
WiFiServer server(80);


String redString = "0";
String greenString = "0";
String blueString = "0";
int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;

// Variable to store the HTTP req  uest
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Conectando ...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Imprimir ip local
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();  // Esperar nuevos clientes

  if (client) {  // nuevo cliente
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("Nuevo cliente");                                             
    String currentLine = "";                                                   
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  
      currentTime = millis();
      if (client.available()) {  // si se reciben datos del cliente,
        char c = client.read();  // leer bytes
        Serial.write(c);         
        header += c;
        if (c == '\n') { 
          if (currentLine.length() == 0) {

            // HTTP headers     
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            //Mostrar pagina html
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">");
            client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>");
            client.println("</head><body><div class=\"container\"><div class=\"row\"><h1>ROBLEX Selector de Color</h1></div>");
            client.println("<a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_color\" role=\"button\">Enviar color</a> ");
            client.println("<input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\"></div>");
            client.println("<script>function update(picker) {document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);");
            client.println("document.getElementById(\"change_color\").href=\"?r\" + Math.round(picker.rgb[0]) + \"g\" +  Math.round(picker.rgb[1]) + \"b\" + Math.round(picker.rgb[2]) + \"&\";}</script></body></html>");
  
            client.println();

            // ejemplo de entrada: /?r201g32b255&
            // Red = 201 | Green = 32 | Blue = 255
            if (header.indexOf("GET /?r") >= 0) {
              pos1 = header.indexOf('r');
              pos2 = header.indexOf('g');
              pos3 = header.indexOf('b');
              pos4 = header.indexOf('&');
              redString = header.substring(pos1 + 1, pos2);
              greenString = header.substring(pos2 + 1, pos3);
              blueString = header.substring(pos3 + 1, pos4);

              ROBLEX.Rgb(redString.toInt(), greenString.toInt(), blueString.toInt());
            }
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;     
        }
      }
    }
   
    header = "";
   
    client.stop();
    Serial.println("Cliente desconectado");
    Serial.println("");
  }
}