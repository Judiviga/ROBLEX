# Protocolo BLE entre la app ROBLEX y el robot

Guía para sesiones de Claude que escriban firmware (sketches de Arduino con la librería ROBLEX) o cambien la app, en las secciones **Seguidor de línea** y **Terminal**. Si algo de aquí no coincide con el código, manda el código: revisa los archivos de la sección [Dónde está cada cosa](#dónde-está-cada-cosa) y actualiza esta guía.

Última revisión: 2026-10-22.

---

## 1. Transporte

| | |
|---|---|
| Servicio | Nordic UART Service `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` |
| RX (app → robot) | `6E400002-…`, escritura **sin respuesta** |
| TX (robot → app) | `6E400003-…`, **notificaciones** |
| Formato | texto UTF-8, **una línea por mensaje** |
| Decimales | siempre con punto (`0.0128`): la coma es el separador de campos |

- **App → robot:** la app agrega `\n` a cada comando. La librería junta los bytes hasta el `\n` (aunque lleguen en varios paquetes) y entrega la línea **sin** el `\n` por `BluetoothRead()`.
- **Robot → app:** cada `ROBLEX.BluetoothWrite(texto)` es **una** notificación, y la app la toma como un mensaje (no hace falta `\n`). Si el texto trae `\n`, la app lo separa en varias líneas. **No partas un mensaje en dos `BluetoothWrite`:** la app los mostraría como dos líneas.
- **Tamaño:** en Android la app pide MTU 512; iOS negocia ~185. Mantén cada mensaje **≤ 180 bytes**.
- **Escaneo:** la app solo lista dispositivos que anuncian el servicio NUS. El nombre que ve el usuario es el que se pasa a `ROBLEX.BluetoothBegin("nombre")`.

### Requisito de la librería (importante)

La versión publicada de la librería ROBLEX (2.0.1) tiene dos fallas que rompen este protocolo. Están corregidas en `src/Bluetooth.cpp` de la rama **`seguidor-de-linea`** de `Judiviga/ROBLEX` (subida el 2026-11-12; `main` todavía no las tiene):

1. **MTU 23:** sin `BLEDevice::setMTU(517)`, el ESP32 queda en 23 bytes, es decir 20 de datos por paquete. `SEN,…` (hasta 41 bytes) llega cortado y la app lo descarta.
2. **Solo la última línea:** antes `BluetoothRead()` guardaba solo la última línea recibida. La app manda comandos seguidos (`SEN,1` + `GET` al abrir el seguidor, `PID,…` + `RUN,1` al iniciar) y se perdía el primero. Ahora es una cola de 8 líneas.

Antes de escribir firmware, verifica que `Bluetooth.cpp` tenga `ROBLEX_BLE_MTU` y `_roblexRxQueue`. `Documents\Arduino\libraries\ROBLEX` es un *junction* a ese clon: arduino-cli y el IDE compilan esa copia. **No actualices ROBLEX desde el Library Manager**, porque eso pisaría el clon.

---

## 2. API de la librería para el firmware

```cpp
ROBLEX.BluetoothBegin("ROBLEX linea");  // en setup(); nombre visible en la app
ROBLEX.BluetoothConnected();            // true si la app está conectada
ROBLEX.BluetoothAvailable();            // hay al menos una línea sin leer
ROBLEX.BluetoothRead();                 // línea más antigua (sin '\n'), o "" si no hay
ROBLEX.BluetoothWrite("texto");         // un mensaje a la app
ROBLEX.ReadApp(cmd);                    // separa cmd por comas en ROBLEX.AppValue[0..]
```

- Cola de recepción: 8 líneas de hasta 255 caracteres. Si se llena, se descarta la más vieja, y se vacía al desconectarse.
- **Lee todo lo pendiente en cada vuelta del loop**, sobre todo si el loop tiene `delay`:
  `while (ROBLEX.BluetoothAvailable()) comandoApp(ROBLEX.BluetoothRead());`
- `MotorForward` y `MotorBackward` reciben el ciclo de trabajo **en % (0-100)**, pero la app maneja velocidades de 0 a 255. Convierte con `duty = abs(out) * 100 / 255`, como hace `Drive()` en `sensor_de_linea`. Si no, todo lo que pase de 100 sale a fondo.

---

## 3. Comandos

### App → robot

| Comando | Cuándo lo manda la app | Qué debe hacer el robot |
|---|---|---|
| `outR,outL,R,G,B` | Joystick, cada 50 ms (20 Hz por defecto, ajustable de 5 a 100 Hz), **solo con la pantalla del joystick abierta**. Al salir manda `0,0,R,G,B`. | Mover motores y LED. Empieza por número o `-`: así se distingue de los demás. |
| `SEN,1` | Al abrir el seguidor de línea, y en cada reconexión con esa pantalla abierta | Empezar a enviar `SEN,…` |
| `SEN,0` | Al salir del seguidor de línea | Dejar de enviar `SEN,…` |
| `GET` | Justo después de `SEN,1`, y con el botón "Leer del robot" | Responder, cada uno en su mensaje: `PID,kp,ki,kd,vel`, `UMB,n`, `LIN,N` o `LIN,B`, y `CAL,1` solo si está calibrando |
| `PID,kp,ki,kd,vel` | Botón "Enviar al robot", y antes de `RUN,1` si hay cambios sin enviar | Validar que tenga 5 campos y aplicar los valores (también en marcha) |
| `UMB,n` | Al **soltar** el umbral (la raya sobre las barras o el control deslizante). Se envía al instante, sin pasar por "Enviar al robot" | Cambiar el umbral (1-254, también en marcha). No se confirma |
| `LIN,N` / `LIN,B` | Al tocar "Negra" o "Blanca" en "Color de la línea". Se envía **al instante**, sin pasar por "Enviar al robot" | Seguir línea negra sobre fondo blanco (`N`) o blanca sobre fondo negro (`B`), y confirmar con el mismo `LIN,X`. Si está en marcha y el color cambia, detenerse |
| `RUN,1` | Botón "Iniciar" | Arrancar **ya**, sin cuenta regresiva |
| `RUN,0` | Botón "Detener" | Parar y frenar motores |
| `CAL,1` | Botón "Calibrar sensores" | Detenerse si estaba en marcha, entrar en **modo calibración** (sin límite de tiempo) y responder `CAL,1`. **No bloquear**: debe seguir atendiendo comandos y enviando `SEN` |
| `CAL,0` | Botón "Terminar calibración" (el mismo botón mientras calibra) | Aplicar la calibración y responder `CAL,0,…` con los sensores sin contraste |
| `CAL` | Solo a mano desde la Terminal (versiones viejas de la app lo mandaban para calibrar 3 s) | Alternar: entrar o terminar el modo calibración |
| texto libre | Terminal | Lo que el sketch decida |

Formato de `PID` que envía la app: `kp`, `ki` y `kd` ≥ 0, con hasta 4 decimales y sin ceros de más. `vel` es un entero de 0 a 255. Ejemplo: `PID,0.0128,0,0.089,128`.

### Robot → app

| Mensaje | Formato | Cómo lo usa la app |
|---|---|---|
| Lectura | `SEN,s0,s1,s2,s3,s4,s5,s6,s7,pos` | 8 barras y marcador de posición |
| Parámetros | `PID,kp,ki,kd,vel` | Reemplaza y **guarda** los valores en la app; muestra "Parámetros recibidos" |
| Color de línea | `LIN,N` o `LIN,B` | Marca la opción en "Color de la línea" y la guarda, sin aviso |
| Umbral | `UMB,n` | Reemplaza y guarda el umbral, sin aviso (solo llega tras `GET`) |
| Calibrando | `CAL,1` | El botón pasa a "Terminar calibración" y las barras muestran el rango que ve cada sensor |
| Calibración terminada | `CAL,0` o `CAL,0,i,j,…` | Vuelve al botón normal y avisa "Calibración lista", o lista los sensores `i, j` que no vieron contraste. Si la app no estaba calibrando, solo actualiza el estado |
| Cualquier otra línea | texto libre | Solo aparece en la Terminal |

`SEN`:
- `s0..s7` son enteros (la app los recorta a 0-255). Convención de la placa ADS7830: **alto = blanco, bajo = negro**, con cualquier color de línea: con `LIN,B` los sensores que ven la línea dan valores **altos**. No inviertas los valores al enviarlos; el color solo cambia cómo el firmware calcula `pos`. Si ya se calibró, manda los valores calibrados.
- `s0` es la barra de la **izquierda** y debe ser el sensor izquierdo del robot mirando hacia adelante. Si la placa está montada al revés, invierte el orden en el firmware (ver `INVERTIR_SENSORES`).
- `pos` es un entero de **-3500** (línea bajo S0) a **3500** (bajo S7), con 0 en el centro. **Omite `pos`** cuando ningún sensor ve la línea: la app oculta el marcador.
- Envía solo mientras `SEN,1` esté activo, cada **50 ms** (20 Hz). Pon el flag en `false` al desconectarse, porque la app lo pide de nuevo al reconectar.
- **Mientras calibras**, envía `SEN` con los valores **sin calibrar** y **sin `pos`**: la app arma con ellos el rango de cada sensor.

`PID`: exactamente 5 campos. Usa `String(x, 4)` para los decimales; la app redondea a 4.

`LIN`: exactamente 2 campos, `N` o `B` (la app también acepta minúscula). Cada `BluetoothWrite` es un mensaje, así que `PID,…`, `UMB,…`, `LIN,…` y `CAL,…` van en llamadas separadas.

`UMB`: exactamente 2 campos, entero. Un sensor ve la línea si su valor (ya calibrado) es **menor que n** con línea negra, o **mayor que 255 - n** con línea blanca. De fábrica es **128**: el sensor está más cerca del color de la línea que del fondo. Supone sensores calibrados; sin calibrar, el blanco puede leerse bajo y contaría como línea. La app usa la misma regla (`LineProtocol.onLine`) para pintar de verde las barras y dibujar la raya del umbral: si cambias la regla en el firmware, cámbiala también ahí.

Posición: el firmware promedia la posición de los sensores que ven la línea, con peso = cuánto pasa cada uno del umbral (`nivelLinea(i) - (255 - umbral)`). Así la posición cambia de forma continua, sin saltos cuando un sensor entra o sale de la línea.

`CAL`:
- `CAL,1` exactamente; `CAL,0` seguido de 0 a 8 índices de sensor (0-7). Sin índices = todos vieron contraste.
- Un sensor tiene contraste si `máximo - mínimo ≥ 30` (`CONTRASTE_MINIMO` en el firmware, `LineProtocol.minContrast` en la app: deben coincidir). Al terminar, los sensores **sin** contraste conservan su calibración anterior.
- Responde `CAL,1` también si ya estaba calibrando, y `CAL,0` (sin índices) si no lo estaba: así la app se resincroniza.
- Si llega `RUN,1` calibrando, no arranques: avisa "Termina la calibración antes de arrancar".

Mensajes de texto: escríbelos en español y para personas ("Calibrando: pasa los sensores sobre la línea", "En marcha", "Detenido", "Comando desconocido: X"). **Nunca empieces un texto libre con `SEN,`, `PID,`, `UMB,`, `LIN,` ni `CAL,`.** Ojo: la pantalla del seguidor de línea **no** muestra estos textos; solo los ve quien abra la Terminal.

---

## 4. Comportamiento de la app

### Seguidor de línea (`lib/pages/line_follower_page.dart`)
- Mientras no llega `GET`, usa los valores guardados en el teléfono. Si nunca recibió ninguno, usa 0.05 / 0 / 0.5 / 150.
- Pasos de +/-: Kp 0.001, Ki 0.0001, Kd 0.01, velocidad 10. Tocar el número permite escribirlo (acepta coma o punto decimal).
- El botón "Enviar al robot" se pone verde cuando hay cambios sin enviar, y envía `PID,…`.
- Barras de sensores (`lib/widgets/sensor_bars.dart`):
  - **Escala automática:** 0-64, 0-128 o 0-255, la más chica en la que caben el valor más alto recibido y la raya del umbral (`barScaleFor`). Se reinicia al conectar y al entrar o salir de la calibración. Así las lecturas bajas sin calibrar se ven grandes.
  - **Umbral:** raya naranja que cruza todas las barras (`LineProtocol.thresholdLevel`), con la zona que cuenta como línea sombreada en verde. La barra se pinta de verde si ese sensor ve la línea. **Arrastrar o tocar sobre las barras mueve la raya**; al soltar se envía `UMB,n`.
  - **Calibrando:** no hay umbral; cada barra muestra un recuadro con el rango visto (armado en la app con los `SEN` que llegan) y el nombre `S3` se pone verde cuando ya tiene contraste.
- "Calibrar sensores" (justo debajo de la posición): manda `CAL,1`, y el botón cambia a "Terminar calibración" (morado) **cuando el robot responde** `CAL,1`, no antes. Con el robot calibrando, "Iniciar" queda deshabilitado. Al desconectar, la app olvida el estado; si el robot seguía calibrando, lo informa en la respuesta a `GET`.
- "Umbral" está justo debajo de las barras: control deslizante de 1 a 254 (tocar el número permite escribirlo), por defecto 128 (igual que el robot). Mientras se mueve, solo cambia la vista; al soltar se guarda en el teléfono y se envía `UMB,n`. Deshabilitado sin conexión y calibrando.
- "Color de la línea" (Negra / Blanca, entre la posición y los parámetros): envía `LIN,X` al tocar, y se actualiza con el `LIN` que responde el robot. Se guarda en el teléfono (`SettingsStorage.loadLineColor`). Un firmware sin `LIN` contesta "Comando desconocido", que solo se ve en la Terminal, y la app sigue mostrando lo elegido.
- Sin conexión, los botones y el selector de color quedan deshabilitados.

### Terminal (`lib/pages/terminal_page.dart`, `lib/models/terminal_log.dart`)
- Escucha desde que arranca la app, así que también guarda lo que llega mientras estás en otra pantalla. Historial de 500 líneas, con hora, `<` para recibido y `>` para enviado.
- Las líneas `SEN,…` **se ocultan por defecto** (botón de sensores para mostrarlas) y, mientras están ocultas, no se guardan.
- Todo lo que envía el seguidor de línea también aparece en la Terminal. El joystick no aparece.
- Envía el texto tal cual más `\n`, en UTF-8. Si el robot lo muestra en la OLED, convierte las tildes y la ñ a CP437 (ver `paraPantalla()` en `BL_terminal`).
- Comandos rápidos: `GET`, `RUN,1`, `RUN,0`, `SEN,1`, `SEN,0`, `CAL,1`, `CAL,0`, `LIN,N`, `LIN,B`.

---

## 5. Esqueleto de firmware

```cpp
#include "ROBLEX.h"
ROBLEX ROBLEX;

String RobotName = "ROBLEX mi robot";
bool enviarSensores = false;
float kp = 0.0128, ki = 0, kd = 0.089;
int velocidad = 128;
int umbral = 128;
bool corriendo = false;
bool calibrando = false;
bool lineaBlanca = false;  // false = línea negra sobre fondo blanco

void comandoApp(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;
  if (isDigit(cmd[0]) || cmd[0] == '-') {  // joystick "outR,outL,R,G,B"
    // mover solo si !corriendo; ver control_remoto_robot para los signos
    return;
  }
  if (cmd == "SEN,1") enviarSensores = true;
  else if (cmd == "SEN,0") enviarSensores = false;
  else if (cmd == "GET") {
    ROBLEX.BluetoothWrite("PID," + String(kp, 4) + "," + String(ki, 4) + "," +
                          String(kd, 4) + "," + String(velocidad));
    ROBLEX.BluetoothWrite("UMB," + String(umbral));          // cada uno en su mensaje
    ROBLEX.BluetoothWrite(lineaBlanca ? "LIN,B" : "LIN,N");
    if (calibrando) ROBLEX.BluetoothWrite("CAL,1");
  } else if (cmd.startsWith("UMB,")) {
    umbral = constrain(cmd.substring(4).toInt(), 1, 254);
  } else if (cmd == "LIN,N" || cmd == "LIN,B") {
    lineaBlanca = (cmd == "LIN,B");  // si estaba en marcha y cambió, detenerse
    ROBLEX.BluetoothWrite(cmd);      // confirmar a la app
  } else if (cmd.startsWith("PID,")) {
    ROBLEX.ReadApp(cmd);  // AppValue[0]="PID", [1]=kp, [2]=ki, [3]=kd, [4]=vel
    kp = ROBLEX.AppValue[1].toFloat();
    ki = ROBLEX.AppValue[2].toFloat();
    kd = ROBLEX.AppValue[3].toFloat();
    velocidad = constrain(ROBLEX.AppValue[4].toInt(), 0, 255);
  } else if (cmd == "RUN,1") corriendo = !calibrando;  // no arrancar calibrando
  else if (cmd == "RUN,0") corriendo = false;  // y frenar motores
  else if (cmd == "CAL,1") {
    calibrando = true;  // detener motores; reiniciar mínimo/máximo de cada sensor
    ROBLEX.BluetoothWrite("CAL,1");
  } else if (cmd == "CAL,0") {
    calibrando = false;  // aplicar solo los sensores con contraste
    ROBLEX.BluetoothWrite("CAL,0");  // + ",i,j" con los sensores sin contraste
  }
  else ROBLEX.BluetoothWrite("Comando desconocido: " + cmd);
}

void setup() {
  ROBLEX.BluetoothBegin(RobotName);
}

void loop() {
  while (ROBLEX.BluetoothAvailable()) comandoApp(ROBLEX.BluetoothRead());
  if (!ROBLEX.BluetoothConnected()) {
    enviarSensores = false;
    // seguridad: frenar lo que movía el joystick
  }
  // PID a ritmo fijo (por ejemplo cada 5 ms con millis()), y cada 50 ms:
  //   if (enviarSensores) ROBLEX.BluetoothWrite("SEN,s0,...,s7,pos");
  //   (calibrando: valores sin calibrar y sin pos)
  // Calibrando, en cada lectura guarda el mínimo y el máximo de cada sensor:
  // nunca dentro de un while que bloquee, porque hay que seguir leyendo la app.
  // Un sensor ve la línea si su valor está a menos de "umbral" del color de la
  // línea: ver nivelLinea() y sobreLinea() en sensor_de_linea.
  // No actualices la OLED mientras corre: tarda ~25 ms y frena el PID.
}
```

La implementación completa y probada en compilación es `examples/ejemplos_robot/sensor_de_linea`.

---

## 6. Dónde está cada cosa

| Qué | Ruta |
|---|---|
| App (Flutter), branch **`app-v2`** | `Documents\ROBLEX\RobotCompetitionController` |
| Protocolo en la app (constructores y parseo) | `lib/models/line_follower.dart` (`LineProtocol`) |
| Pruebas del protocolo | `test/line_protocol_test.dart` (`flutter test`) |
| Transporte BLE de la app | `lib/models/bluetooth_interface.dart` |
| Librería Arduino ROBLEX | `Documents\ROBLEX\ROBLEX` (`src/Bluetooth.cpp`, `src/Line.cpp`) |
| Seguidor completo (referencia) | `ROBLEX/examples/ejemplos_robot/sensor_de_linea/` |
| Ejemplo de Terminal | `ROBLEX/examples/comunicaciones/BL_terminal/` |
| Joystick (referencia de signos) | `ROBLEX/examples/ejemplos_robot/control_remoto_robot/` |
| Sensor de línea (hardware) | `Documents\ROBLEX\electronica\sensor de linea` (8× QRE1113 + ADS7830 en 0x48) |

Compilar:
- Firmware: `arduino-cli compile --fqbn esp32:esp32:esp32 <carpeta del sketch>`. Tarda unos 5 minutos en este PC: córrelo en segundo plano.
- App: `flutter build apk --release` con `JAVA_HOME=C:\Users\Owner\develop\jdk-21`. Tarda unos 10 minutos.

---

## 7. Si cambias el protocolo

- Cambia **las dos puntas** en el mismo trabajo: `LineProtocol` (con sus pruebas) y el firmware de referencia, y actualiza esta guía.
- Todo comando nuevo hacia el robot debe empezar con **letras**. Lo que empieza por número o `-` es del joystick.
- No agregues campos a `SEN` ni a `PID` sin revisar el parseo de la app: `SEN` necesita al menos 9 campos y `PID` exactamente 5.

## 8. Limitaciones conocidas (el usuario decidió no cambiarlas por ahora)

No las "arregles" sin que el usuario lo pida:
- Desde 2026-10-22 el umbral de fábrica es 128 (antes 20, que perdía la línea entre dos sensores) y supone sensores calibrados. Sin calibrar, con la señal débil de esta placa (blanco estimado en 50-70 según el circuito, sin medir), el robot puede ver línea en todos los sensores: hay que calibrar o bajar el umbral. La sensibilidad real de la señal la limita el hardware (referencia interna de 2.5 V, 8 bits, R9-R16 de 3.3 kΩ); promediar varias lecturas no la mejora, porque el ADC casi no tiene ruido (0.02 LSB).
- El PID, el umbral, la calibración y el color de línea no se guardan en el robot. Después de un reinicio, `GET` devuelve los valores de fábrica (línea negra, umbral 128) y la app los guarda encima de los que el usuario afinó.
- Los valores por defecto de la app (0.05 / 0 / 0.5 / 150) no coinciden con los del robot (0.0128 / 0 / 0.089 / 128).
- La pantalla del seguidor de línea no muestra los mensajes de texto del robot; solo los muestra la Terminal.
