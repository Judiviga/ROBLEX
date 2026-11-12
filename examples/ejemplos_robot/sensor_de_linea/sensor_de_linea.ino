#include "ROBLEX.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Seguidor de linea con el modulo sensor de linea (8 sensores QRE1113 + ADC ADS7830)
// Lee los 8 sensores, calcula la posicion de la linea y, en marcha, ajusta la
// velocidad de cada rueda con un control PID para mantener la linea en el centro.
//   valor alto (hasta 255) = superficie blanca
//   valor bajo             = superficie negra (o sensor levantado, sin nada debajo)
// Sigue una linea negra sobre fondo blanco o, si se elige en la app, una linea blanca
// sobre fondo negro. Los valores de los sensores no cambian: solo cambia cual color es la linea.
// Detenido muestra los 8 sensores como barras en la pantalla oled y en el debug serial.
// Led RGB: verde = ve la linea, azul = no la ve, morado = calibrando,
//          amarillo = a punto de arrancar, rojo = error.
// Abrir el Serial Plotter a 115200 para ver las 8 curvas.
//
// Calibracion: el robot queda en modo calibracion hasta que se termina. Mientras tanto
// hay que pasar todos los sensores sobre la linea y sobre el fondo (a mano o con el
// joystick); cada sensor guarda el valor mas oscuro y el mas claro que vio. Al terminar,
// los sensores que no vieron suficiente contraste conservan su calibracion anterior.
//
// Boton de la shield:
//   toque corto          arrancar (2 segundos despues, para retirar la mano) o detener
//   mantener 1 segundo   entrar en modo calibracion; otro toque la termina
//
// Tambien se controla por BLE con la app ROBLEX (seccion "Seguidor de linea"):
//   robot -> app: "SEN,s0,s1,...,s7,pos"  (pos de -3500 a 3500; sin pos si no hay linea
//                                          o calibrando, que envia los valores sin calibrar)
//                 "PID,kp,ki,kd,vel"      respuesta a "GET"
//                 "UMB,n"                 umbral: respuesta a "GET"
//                 "LIN,N" / "LIN,B"       color de la linea: respuesta a "GET" y a "LIN"
//                 "CAL,1"                 entro en modo calibracion (y respuesta a "GET" si sigue)
//                 "CAL,0,i,j..."          termino la calibracion; i, j = sensores sin contraste
//   app -> robot: "SEN,1" / "SEN,0"       activar / apagar el envio de sensores
//                 "PID,kp,ki,kd,vel"      cambiar el PID y la velocidad base (0-255)
//                 "UMB,n"                 cambiar el umbral (1-254)
//                 "LIN,N" / "LIN,B"       seguir linea negra (fondo blanco) / blanca (fondo negro)
//                 "GET"                   pedir PID, umbral, color de la linea y estado de calibracion
//                 "RUN,1" / "RUN,0"       arrancar / detener
//                 "CAL,1" / "CAL,0"       entrar / terminar el modo calibracion ("CAL" alterna)
//                 "outR,outL,R,G,B"       joystick: mueve el robot mientras no sigue la linea
// Los mensajes de texto (por ejemplo al calibrar) se ven en la terminal de la app.

// CONECTAR EL MODULO SENSOR DE LINEA Y LA PANTALLA OLED AL BUS I2C DE LA SHIELD
// conector del modulo: 1 GND, 2 3.3V, 3 SCL, 4 SDA
// CONECTAR EL MODULO DRIVER DE MOTOR IZQUIERDO EN EL PUERTO 4 Y EL DERECHO EN EL PUERTO 5
// (robot de 4 motores: los otros dos en el puerto 1 el izquierdo y en el 3 el derecho)

#define LINEA_DIRECCION 0x48     // direccion I2C del modulo (0x48 de fabrica)
#define CONTRASTE_MINIMO 30      // al calibrar, diferencia minima entre el valor mas oscuro y el mas claro
#define INVERTIR_SENSORES false  // true si el sensor 0 queda a la DERECHA del robot (mirando hacia adelante)

#define RIGHT_PWM MCPWM_UNIT_0
#define LEFT_PWM MCPWM_UNIT_1

#define PERIODO_CONTROL 5        // ms entre correcciones del PID
#define PERIODO_ENVIO 50         // ms entre envios a la app, al debug serial y a la pantalla
#define TIEMPO_BOTON_LARGO 1000  // ms manteniendo el boton para calibrar

ROBLEX ROBLEX;
Adafruit_SSD1306 display(128, 64, &Wire, -1, 400000, 400000);  // configurar la pantalla oled (bus I2C a 400 kHz)

String RobotName = "ROBLEX linea";  // nombre del robot en el bluetooth

int crudo[8];     // valor de cada sensor sin calibrar (0-255)
int sensores[8];  // valor de cada sensor calibrado: 0 = su negro, 255 = su blanco

// calibracion de cada sensor: su valor mas oscuro y el mas claro
// (sin calibrar, 0 y 255: los valores calibrados son iguales a los crudos)
int minimo[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int maximo[8] = {255, 255, 255, 255, 255, 255, 255, 255};
bool calibrado = false;  // true si al menos un sensor ya se calibro

bool calibrando = false;  // true mientras el robot esta en modo calibracion
int calMinimo[8];         // lo visto en la calibracion en curso
int calMaximo[8];

// un sensor esta sobre la linea si su valor esta a menos de "umbral" del color de la linea
// (con linea negra: valor menor que umbral; con linea blanca: mayor que 255 - umbral).
// 128 = el sensor esta mas cerca del color de la linea que del fondo. Mas alto = mas
// sensible. Supone sensores calibrados: sin calibrar, el blanco puede leerse bajo y
// contar como linea; en ese caso calibra o baja el umbral.
int umbral = 128;  // la app lo cambia con "UMB,n"

bool enviarSensores = false;  // la app lo activa con "SEN,1" al abrir el seguidor de linea
bool lineaBlanca = false;     // false = linea negra sobre blanco; la app lo cambia con "LIN,N" / "LIN,B"

// parametros del PID; la app los cambia con "PID,kp,ki,kd,vel"
// (los del programa seguidor_de_linea pasados a velocidades de 0 a 255)
float kp = 0.0128;    // proporcional: corrige mas cuanto mas lejos del centro esta la linea
float ki = 0;         // integral: corrige un error que se mantiene en el tiempo
float kd = 0.089;     // derivativo: suaviza la correccion cuando la linea se mueve rapido
int velocidad = 128;  // velocidad base de las ruedas (0-255)

bool corriendo = false;  // true mientras el robot sigue la linea
float integral = 0;      // suma de los errores (termino integral)
int ultimoError = 0;     // error del ciclo anterior (termino derivativo)
int ultimaPosicion = 0;  // ultima posicion vista, para saber por que lado se salio la linea

unsigned long ultimoControl = 0;
unsigned long ultimoEnvio = 0;

// busca dispositivos en el bus I2C e imprime sus direcciones en el debug serial
void buscarI2C() {
  Serial.print("Dispositivos I2C encontrados:");
  for (uint8_t direccion = 1; direccion < 127; direccion++) {
    Wire.beginTransmission(direccion);
    if (Wire.endTransmission() == 0) {
      Serial.print(" 0x");
      Serial.print(direccion, HEX);
    }
  }
  Serial.println();
}

// envia un mensaje de texto a la app (se ve en la terminal) y al debug serial
void mensaje(String texto) {
  Serial.println(texto);
  ROBLEX.BluetoothWrite(texto);
}

// arma "SEN,s0,...,s7" con los valores de los 8 sensores
String lineaSensores(int *valores) {
  String linea = "SEN";
  for (int i = 0; i < 8; i++) {
    linea += "," + String(valores[i]);
  }
  return linea;
}

//crear funcion para mover los motores: out de -255 a 255, 0 frena el motor
void Drive(mcpwm_unit_t unit, int out) {

  int duty = abs(out) * 100 / 255;  // la libreria recibe el ciclo de trabajo en % (0-100)

  if (out < 0) {
    ROBLEX.MotorForward(unit, duty);

  } else if (out > 0) {
    ROBLEX.MotorBackward(unit, duty);

  } else {
    ROBLEX.MotorStop(unit);
  }
}

// mueve las dos ruedas de -255 (atras) a 255 (adelante)
// los motores van en espejo, por eso los signos opuestos; si una rueda gira al reves, cambia su signo aqui
void motores(int derecha, int izquierda) {
  Drive(RIGHT_PWM, -derecha);
  Drive(LEFT_PWM, izquierda);
}

void setup() {

  Serial.begin(115200);  // iniciar debug serial
  Serial.println("start");

  //Configurar los motores en los puertos respectivos con una frecuencia de pwm de 20000
  ROBLEX.SetupMotor(4, LEFT_PWM, 20000);
  ROBLEX.SetupMotor(5, RIGHT_PWM, 20000);

  // configuracion para robot de 4 motores
  ROBLEX.SetupMotor(1, LEFT_PWM, 20000);
  ROBLEX.SetupMotor(3, RIGHT_PWM, 20000);

  motores(0, 0);  // empezar con los motores frenados

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // iniciar la pantalla oled
  display.setTextColor(WHITE);
  Wire.setClock(400000);  // I2C a 400 kHz para leer los sensores mas rapido

  // iniciar el modulo sensor de linea; si no responde, reintentar cada segundo
  while (!ROBLEX.SetupLine(LINEA_DIRECCION)) {
    ROBLEX.Rgb(255, 0, 0);  // led rojo: modulo no encontrado

    Serial.print("No se encontro el sensor de linea en 0x");
    Serial.println(LINEA_DIRECCION, HEX);
    buscarI2C();  // ayuda a ver si el modulo esta en otra direccion o no conecta

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Sensor de linea");
    display.println("no encontrado");
    display.print("en 0x");
    display.println(LINEA_DIRECCION, HEX);
    display.display();

    delay(1000);
  }
  Serial.println("Sensor de linea listo");

  ROBLEX.BluetoothBegin(RobotName);  //iniciar la comunicacion BLE con el nombre asignado
  Serial.print("Direccion BLE: ");
  Serial.println(ROBLEX.BluetoothAddress());
}

// lee los 8 sensores sin calibrar, con el sensor 0 a la izquierda del robot
bool leerCrudo(int *valores) {
  if (!ROBLEX.LineValues(valores)) {
    return false;
  }
  if (INVERTIR_SENSORES) {
    for (int i = 0; i < 4; i++) {
      int temporal = valores[i];
      valores[i] = valores[7 - i];
      valores[7 - i] = temporal;
    }
  }
  return true;
}

// lee los 8 sensores y escala cada uno de su negro (0) a su blanco (255);
// en modo calibracion, ademas guarda el valor mas oscuro y el mas claro de cada sensor
bool leerSensores() {
  if (!leerCrudo(crudo)) {
    return false;
  }
  for (int i = 0; i < 8; i++) {
    sensores[i] = constrain(map(crudo[i], minimo[i], maximo[i], 0, 255), 0, 255);
    if (calibrando) {
      calMinimo[i] = min(calMinimo[i], crudo[i]);
      calMaximo[i] = max(calMaximo[i], crudo[i]);
    }
  }
  return true;
}

// true si en la calibracion en curso el sensor i ya vio suficiente diferencia entre oscuro y claro
bool tieneContraste(int i) {
  return calMaximo[i] - calMinimo[i] >= CONTRASTE_MINIMO;
}

// entra en modo calibracion: dura hasta terminarCalibracion()
void empezarCalibracion() {
  if (calibrando) {
    ROBLEX.BluetoothWrite("CAL,1");  // ya estaba calibrando: solo confirmar a la app
    return;
  }
  if (corriendo) {
    detener();  // no calibrar con los motores en marcha
  }
  for (int i = 0; i < 8; i++) {
    calMinimo[i] = 255;
    calMaximo[i] = 0;
  }
  calibrando = true;

  ROBLEX.Rgb(255, 0, 255);  // led morado: calibrando
  ROBLEX.BluetoothWrite("CAL,1");
  mensaje("Calibrando: pasa todos los sensores sobre la linea y sobre el fondo, y vuelve a tocar calibrar para terminar");
}

// termina el modo calibracion: cada sensor que vio suficiente contraste toma su nuevo
// negro y blanco; los demas se quedan como estaban
void terminarCalibracion() {
  if (!calibrando) {
    ROBLEX.BluetoothWrite("CAL,0");  // no estaba calibrando: solo confirmar a la app
    return;
  }
  calibrando = false;

  String respuesta = "CAL,0";   // para la app: "CAL,0,i,j..." con los sensores sin contraste
  String sinContraste = "";     // para la terminal y el debug serial
  for (int i = 0; i < 8; i++) {
    if (tieneContraste(i)) {
      minimo[i] = calMinimo[i];
      maximo[i] = calMaximo[i];
      calibrado = true;
    } else {
      respuesta += "," + String(i);
      sinContraste += " S" + String(i);
    }
  }

  ROBLEX.BluetoothWrite(respuesta);
  if (sinContraste == "") {
    mensaje("Calibracion lista");
  } else {
    mensaje("Calibracion lista. Sin contraste (quedan como estaban):" + sinContraste);
  }
}

// pantalla mientras el robot esta en marcha: se dibuja una sola vez al arrancar,
// porque actualizar la pantalla tarda y retrasaria las correcciones del PID
void pantallaEnMarcha() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 0);
  display.print("EN MARCHA");

  display.setTextSize(1);
  display.setCursor(20, 24);
  display.print("kp: ");
  display.print(kp, 4);
  display.setCursor(20, 34);
  display.print("ki: ");
  display.print(ki, 4);
  display.setCursor(20, 44);
  display.print("kd: ");
  display.print(kd, 4);
  display.setCursor(20, 54);
  display.print("vel: ");
  display.print(velocidad);
  display.display();
}

void arrancar() {
  if (calibrando) {
    mensaje("Termina la calibracion antes de arrancar");
    return;
  }
  if (!calibrado) {
    mensaje("Aviso: sensores sin calibrar; el umbral supone sensores calibrados");
  }
  integral = 0;
  ultimoError = ultimaPosicion;  // evita un golpe del termino derivativo al arrancar
  corriendo = true;
  mensaje("En marcha");
  pantallaEnMarcha();
}

void detener() {
  corriendo = false;
  motores(0, 0);
  mensaje("Detenido");
}

// control PID: calcula la correccion segun el error (posicion de la linea) y mueve las ruedas
void seguirLinea(int posicion) {

  int error = posicion;  // 0 = linea en el centro; negativo = linea a la izquierda

  integral += error;
  if (ki > 0) {
    float limite = 255 / ki;  // que el termino integral no pase de 255 (anti-windup)
    integral = constrain(integral, -limite, limite);
  } else {
    integral = 0;
  }

  float salida = kp * error + ki * integral + kd * (error - ultimoError);
  ultimoError = error;

  // linea a la izquierda (salida negativa): la rueda derecha acelera y la izquierda frena
  int derecha = constrain(velocidad - salida, -255.0, 255.0);
  int izquierda = constrain(velocidad + salida, -255.0, 255.0);
  motores(derecha, izquierda);
}

// responde a "GET" con "PID,kp,ki,kd,vel"
void enviarPID() {
  ROBLEX.BluetoothWrite("PID," + String(kp, 4) + "," + String(ki, 4) + "," + String(kd, 4) + "," + String(velocidad));
}

// "PID,kp,ki,kd,vel": cambia el PID y la velocidad base (tambien en marcha)
void cambiarPID(String cmd) {
  int comas = 0;
  for (unsigned int i = 0; i < cmd.length(); i++) {
    if (cmd[i] == ',') {
      comas++;
    }
  }
  if (comas != 4) {
    mensaje("Uso: PID,kp,ki,kd,vel");
    return;
  }

  ROBLEX.ReadApp(cmd);  //leer el comando y asignar las siguientes variables
  /*
    ROBLEX.AppValue[0]  = "PID"
    ROBLEX.AppValue[1]  = kp
    ROBLEX.AppValue[2]  = ki
    ROBLEX.AppValue[3]  = kd
    ROBLEX.AppValue[4]  = velocidad base (0-255)
  */
  kp = ROBLEX.AppValue[1].toFloat();
  ki = ROBLEX.AppValue[2].toFloat();
  kd = ROBLEX.AppValue[3].toFloat();
  velocidad = constrain(ROBLEX.AppValue[4].toInt(), 0, 255);
  integral = 0;

  Serial.println("PID kp " + String(kp, 4) + " ki " + String(ki, 4) + " kd " + String(kd, 4) + " vel " + String(velocidad));
}

// "UMB,n": cambia el umbral (1-254, tambien en marcha)
void cambiarUmbral(String cmd) {
  String valor = cmd.substring(4);
  valor.trim();
  if (valor.length() == 0 || !isDigit(valor[0]) || valor.indexOf(',') >= 0) {
    mensaje("Uso: UMB,n (n de 1 a 254)");
    return;
  }
  umbral = constrain(valor.toInt(), 1, 254);
  Serial.println("Umbral " + String(umbral));
}

// informa a la app el color de la linea: "LIN,N" negra o "LIN,B" blanca
void enviarColorLinea() {
  ROBLEX.BluetoothWrite(lineaBlanca ? "LIN,B" : "LIN,N");
}

// "LIN,N" / "LIN,B": cambia el color de la linea que sigue el robot
void cambiarColorLinea(bool blanca) {
  if (blanca != lineaBlanca && corriendo) {
    detener();  // con el otro color el robot veria el fondo como linea
  }
  lineaBlanca = blanca;
  Serial.println(lineaBlanca ? "Linea blanca sobre fondo negro" : "Linea negra sobre fondo blanco");
  enviarColorLinea();  // confirmar a la app
}

// atiende un comando de la app
void comandoApp(String cmd) {
  cmd.trim();

  if (cmd.length() == 0) {
    return;
  }
  // joystick de la app: "outR,outL,R,G,B" (empieza por numero o '-')
  // mueve el robot solo si no esta siguiendo la linea (sirve tambien para calibrar);
  // el led sigue mostrando el estado del seguidor
  if (isDigit(cmd[0]) || cmd[0] == '-') {
    if (!corriendo) {
      ROBLEX.ReadApp(cmd);  //leer la aplicacion y asigna las siguientes variables
      /*
        ROBLEX.AppValue[0]  = valor del motor derecho
        ROBLEX.AppValue[1]  = valor del motor izquierdo (la app lo envia con el signo invertido)
      */
      motores(ROBLEX.AppValue[0].toInt(), -ROBLEX.AppValue[1].toInt());
    }
    return;
  }

  if (cmd == "SEN,1") {
    enviarSensores = true;
  } else if (cmd == "SEN,0") {
    enviarSensores = false;
  } else if (cmd == "CAL,1") {
    empezarCalibracion();
  } else if (cmd == "CAL,0") {
    terminarCalibracion();
  } else if (cmd == "CAL") {  // alterna, para escribirlo a mano en la terminal
    if (calibrando) {
      terminarCalibracion();
    } else {
      empezarCalibracion();
    }
  } else if (cmd == "GET") {
    enviarPID();
    ROBLEX.BluetoothWrite("UMB," + String(umbral));
    enviarColorLinea();
    if (calibrando) {
      ROBLEX.BluetoothWrite("CAL,1");  // la app se reconecto en medio de una calibracion
    }
  } else if (cmd.startsWith("PID,")) {
    cambiarPID(cmd);
  } else if (cmd.startsWith("UMB")) {
    cambiarUmbral(cmd);
  } else if (cmd == "LIN,N") {
    cambiarColorLinea(false);
  } else if (cmd == "LIN,B") {
    cambiarColorLinea(true);
  } else if (cmd.startsWith("LIN")) {
    mensaje("Uso: LIN,N (linea negra) o LIN,B (linea blanca)");
  } else if (cmd == "RUN,1") {
    if (!corriendo) {
      arrancar();
    }
  } else if (cmd == "RUN,0") {
    detener();
  } else {
    mensaje("Comando desconocido: " + cmd);
  }
}

// atiende todos los comandos pendientes: la app puede mandar varios seguidos
// (por ejemplo "PID,..." y "RUN,1", o el joystick varias veces por segundo)
void leerApp() {
  while (ROBLEX.BluetoothAvailable()) {
    comandoApp(ROBLEX.BluetoothRead());
  }
}

// espera a que se suelte el boton de la shield
void esperarSoltarBoton() {
  while (digitalRead(boton)) {
    delay(10);
  }
  delay(50);  // antirrebote
}

// boton de la shield: toque corto = arrancar o detener, mantener 1 segundo = calibrar;
// en modo calibracion, cualquier toque la termina
void leerBoton() {
  if (!digitalRead(boton)) {
    return;
  }

  if (corriendo) {  // detener apenas se presiona
    detener();
    esperarSoltarBoton();
    return;
  }

  if (calibrando) {  // terminar la calibracion apenas se presiona
    terminarCalibracion();
    esperarSoltarBoton();
    return;
  }

  unsigned long inicio = millis();
  bool largo = false;
  while (digitalRead(boton)) {
    if (!largo && millis() - inicio >= TIEMPO_BOTON_LARGO) {
      largo = true;
      ROBLEX.Rgb(255, 0, 255);  // led morado: al soltar empieza la calibracion
    }
    delay(10);
  }
  delay(50);  // antirrebote

  if (largo) {
    empezarCalibracion();  // empieza al soltar, con el robot ya en posicion
  } else {
    ROBLEX.Rgb(255, 255, 0);  // led amarillo: a punto de arrancar
    mensaje("Arranca en 2 segundos");
    delay(2000);  // tiempo para retirar la mano
    arrancar();
  }
}

// envia "SEN,s0,...,s7,pos" a la app; pos va de -3500 a 3500 como espera la app
// calibrando envia los valores sin calibrar y sin pos, para ver lo que mide cada sensor
void enviarApp(bool hayLinea, int posicion) {
  if (!ROBLEX.BluetoothConnected()) {
    enviarSensores = false;  // en la proxima conexion la app vuelve a pedirlo
    return;
  }
  if (!enviarSensores) {
    return;
  }
  String linea = lineaSensores(calibrando ? crudo : sensores);
  if (hayLinea && !calibrando) {
    linea += "," + String(posicion);
  }
  ROBLEX.BluetoothWrite(linea);
}

// cuanto ve la linea el sensor i: 255 justo sobre la linea, 0 sobre el fondo
// (los sensores dan alto en blanco y bajo en negro, asi que con linea blanca se invierte)
int nivelLinea(int i) {
  return lineaBlanca ? sensores[i] : 255 - sensores[i];
}

// true si el sensor i esta sobre la linea
bool sobreLinea(int i) {
  return nivelLinea(i) > 255 - umbral;
}

// calcula la posicion de la linea de -3500 (bajo el sensor 0) a 3500 (bajo el sensor 7), 0 = centro
// devuelve false si ningun sensor ve la linea
bool posicionLinea(int &posicion) {
  long suma = 0;
  long pesos = 0;
  for (int i = 0; i < 8; i++) {
    if (sobreLinea(i)) {
      // el peso es cuanto pasa el sensor del umbral: asi la posicion cambia de forma
      // continua, sin saltos cuando un sensor entra o sale de la linea
      int peso = nivelLinea(i) - (255 - umbral);
      suma += (long)peso * (i * 1000 - 3500);
      pesos += peso;
    }
  }
  if (pesos == 0) {
    return false;
  }
  posicion = suma / pesos;
  return true;
}

void dibujar(bool hayLinea, int posicion) {

  display.clearDisplay();  // limpia la informacion del display
  display.setTextSize(1);  // tamano de la letra

  // calibrando: barras con los valores sin calibrar; numero resaltado = sensor con contraste
  int *valores = calibrando ? crudo : sensores;

  // estado, color y posicion de la linea en la parte superior
  display.setCursor(0, 0);
  if (calibrando) {
    display.print("Calibrando");
  } else {
    display.print(lineaBlanca ? "Blanca " : "Negra ");
    if (hayLinea) {
      display.print(posicion);
      int x = map(posicion, -3500, 3500, 6, 118);               // alineado con el centro de las barras 0 a 7
      display.fillTriangle(x, 9, x + 4, 9, x + 2, 13, WHITE);  // marcador de la posicion
    } else {
      display.print("sin linea");
    }
  }

  // "BT" arriba a la derecha mientras la app esta conectada
  if (ROBLEX.BluetoothConnected()) {
    display.setCursor(116, 0);
    display.print("BT");
  }

  // una barra por sensor: mas alta = superficie mas clara
  for (int i = 0; i < 8; i++) {
    int x = i * 16;
    int alto = map(valores[i], 0, 255, 0, 40);
    display.drawRect(x + 2, 15, 12, 42, WHITE);           // marco de la barra
    display.fillRect(x + 3, 56 - alto, 10, alto, WHITE);  // valor del sensor

    // numero del sensor; resaltado si esta sobre la linea (o, calibrando, si ya tiene contraste)
    if (calibrando ? tieneContraste(i) : sobreLinea(i)) {
      display.fillRect(x + 2, 57, 12, 7, WHITE);
      display.setTextColor(BLACK);
    }
    display.setCursor(x + 6, 57);
    display.print(i);
    display.setTextColor(WHITE);
  }

  display.display();  // imprime la informacion en la pantalla
}

void loop() {

  leerApp();    // comandos de la app (SEN, PID, UMB, LIN, GET, RUN, CAL y joystick)
  leerBoton();  // arrancar, detener o calibrar con el boton de la shield

  // leer los sensores y corregir siempre al mismo ritmo, para que kp, ki y kd
  // se comporten igual sin importar lo que tarde el resto del loop
  if (millis() - ultimoControl < PERIODO_CONTROL) {
    return;
  }
  ultimoControl = millis();

  if (!leerSensores()) {  // leer los 8 sensores
    if (corriendo) {
      detener();  // sin sensores no se puede seguir la linea
    }
    Serial.println("Error leyendo el sensor de linea");
    ROBLEX.Rgb(255, 0, 0);
    delay(500);
    return;
  }

  int posicion = 0;
  bool hayLinea = posicionLinea(posicion);
  if (hayLinea) {
    ultimaPosicion = posicion;
  }

  if (corriendo) {
    if (hayLinea) {
      seguirLinea(posicion);
    } else {
      // si no se ve la linea se asume que salio por el ultimo lado en el que estuvo: girar hacia ese lado
      seguirLinea(ultimaPosicion < 0 ? -3500 : 3500);
    }
  }

  // led, debug serial, app y pantalla: mas despacio que el control
  if (millis() - ultimoEnvio < PERIODO_ENVIO) {
    return;
  }
  ultimoEnvio = millis();

  if (!corriendo && !ROBLEX.BluetoothConnected()) {
    motores(0, 0);  // seguridad: si se pierde la conexion, frenar lo que movia el joystick
  }

  if (calibrando) {
    ROBLEX.Rgb(255, 0, 255);  // morado: calibrando
  } else if (hayLinea) {
    ROBLEX.Rgb(0, 255, 0);  // verde: hay linea
  } else {
    ROBLEX.Rgb(0, 0, 255);  // azul: no hay linea
  }

  // imprime los valores en el debug serial (formato del Serial Plotter)
  int *valores = calibrando ? crudo : sensores;
  for (int i = 0; i < 8; i++) {
    Serial.print(valores[i]);
    Serial.print(" ");
  }
  Serial.print("posicion:");
  Serial.println(hayLinea ? posicion : 0);

  enviarApp(hayLinea, posicion);
  if (!corriendo) {
    dibujar(hayLinea, posicion);  // en marcha no: la pantalla tarda y frenaria el PID
  }
}
