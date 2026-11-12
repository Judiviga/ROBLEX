# Seguidor de línea ROBLEX: estado del proyecto

Documento para continuar el desarrollo. Estado al **2026-11-12**.

El proyecto tiene tres partes que se hablan por Bluetooth (BLE):

1. **Robot:** ESP32 sobre la shield ROBLEX, con un módulo sensor de línea de 8 sensores QRE1113 leídos por un ADC ADS7830 por I2C, pantalla OLED, LED RGB y 2 o 4 motores.
2. **Firmware:** un sketch de Arduino que usa la librería ROBLEX. Hace PID, calibra y habla con la app.
3. **App Flutter** (Android/iOS): secciones *Joystick*, *Seguidor de línea* y *Terminal*.

El protocolo entre la app y el robot está en **`PROTOCOLO_APP.md`**, en la misma carpeta que este documento. Léelo antes de tocar cualquiera de las dos puntas.

---

## 1. Dónde está el código

| Qué | En este PC | En GitHub |
|---|---|---|
| **Firmware del seguidor** | `C:\Users\Owner\Documents\ROBLEX\ROBLEX\examples\ejemplos_robot\sensor_de_linea\sensor_de_linea.ino` | repo `Judiviga/ROBLEX`, rama **`seguidor-de-linea`** (ver §2) |
| Lectura del sensor (ADS7830) | `ROBLEX\src\Line.cpp` (API en `ROBLEX\src\ROBLEX.h`: `SetupLine`, `LineValue`, `LineValues`) | ídem |
| BLE de la librería | `ROBLEX\src\Bluetooth.cpp` | ídem |
| Ejemplo de la Terminal | `ROBLEX\examples\comunicaciones\BL_terminal\BL_terminal.ino` | ídem |
| **App** | `C:\Users\Owner\Documents\ROBLEX\RobotCompetitionController`, rama **`app-v2`** | repo `Judiviga/RobotCompetitionController`, rama **`app-v2`** |
| Pantalla del seguidor | `lib/pages/line_follower_page.dart` | |
| Barras de sensores y umbral | `lib/widgets/sensor_bars.dart` | |
| Protocolo en la app | `lib/models/line_follower.dart` (`LineProtocol`) | |
| Terminal | `lib/pages/terminal_page.dart`, `lib/models/terminal_log.dart` | |
| BLE de la app | `lib/models/bluetooth_interface.dart` | |
| Pruebas | `test/line_protocol_test.dart` | |
| APK de release (2.0.0) | `RobotCompetitionController\build\app\outputs\flutter-apk\app-release.apk` (compilado el 2026-10-22, incluye todo lo de abajo) | |
| Protocolo BLE | `C:\Users\Owner\Documents\ROBLEX\ROBLEX\extras\PROTOCOLO_APP.md` | `extras/PROTOCOLO_APP.md` de la librería, rama `seguidor-de-linea` |
| Este documento | `ROBLEX\extras\ESTADO_SEGUIDOR_DE_LINEA.md` | ídem |
| Hardware del sensor | `C:\Users\Owner\Documents\ROBLEX\electronica\sensor de linea\` (EasyEDA, BOM, netlist) | no está en git |

> El sketch viejo de `Documents\ROBLEX\seguidor_de_linea\` (sensor digital PCF8574 y Bluetooth Clásico) **ya no se usa**. El seguidor actual es el ejemplo `sensor_de_linea` de la librería.

---

## 2. Ramas en GitHub (leer primero)

Todo el trabajo del seguidor quedó subido el 2026-11-12. **No está en las ramas principales**:

```bash
git clone -b seguidor-de-linea https://github.com/Judiviga/ROBLEX.git
git clone -b app-v2 https://github.com/Judiviga/RobotCompetitionController.git
```

**Librería `Judiviga/ROBLEX`, rama `seguidor-de-linea`** (commit `ab73456`, sale de `main`):
- `src/Line.cpp` (nuevo) y los cambios en `src/ROBLEX.h`: lectura del ADS7830.
- `src/Bluetooth.cpp`: MTU 517 y cola de 8 líneas al recibir. Sin esto el protocolo falla (ver `PROTOCOLO_APP.md` §1).
- `examples/ejemplos_robot/sensor_de_linea/` (nuevo): el seguidor completo.
- `examples/comunicaciones/BL_terminal/` (nuevo).
- `main` y `library.properties` (versión 2.0.1) no cambiaron: fusionar y subir la versión queda pendiente (§5).

**App `Judiviga/RobotCompetitionController`:**
- **`app-v2`** (versión 2.0.0, último commit `cb85785`): inicio, seguidor, terminal, umbral (`UMB`), color de línea (`LIN`), modo calibración (`CAL,1/0`), barras con escala automática, textos es/en y pruebas.
- `code` (de donde sale `app-v2`): migración a Gradle 9 y MTU 512. `main` no cambió.

**Documentación:** este documento y `PROTOCOLO_APP.md` están en `extras/` de la librería (rama `seguidor-de-linea`). Si cambias el protocolo o el estado, actualízalos ahí mismo.

---

## 3. Qué está hecho y qué está probado

### Firmware (`sensor_de_linea.ino`, unas 700 líneas)
- Lee los 8 sensores por I2C a 400 kHz y calibra por sensor (mínimo y máximo), con un **modo calibración sin límite de tiempo**. Los sensores que no ven contraste conservan su calibración anterior.
- Posición de -3500 a 3500, promediada con peso según cuánto pasa cada sensor del umbral (por defecto 128). Funciona con línea negra o blanca (`LIN`).
- PID cada 5 ms (por defecto kp 0.0128, ki 0, kd 0.089, velocidad 128), con anti-windup. Si pierde la línea, gira hacia el último lado donde la vio.
- Joystick de la app solo cuando no está siguiendo la línea. Frena si se pierde la conexión.
- Botón de la shield: toque corto arranca (con 2 s de espera) o detiene; mantener 1 s entra en calibración y otro toque la termina.
- OLED (solo detenido, para no frenar el PID), LED de estado y Serial Plotter a 115200.

**Probado:**
- Compila para ESP32 (core 3.3.x, 88% de la flash).
- **Cargado en la placa real el 2026-11-06:** arranca, encuentra el sensor en 0x48 y la OLED en 0x3C, y anuncia el BLE.

**Sin probar:**
- Lecturas del sensor sobre una superficie real. Con el sensor **en el aire** daba 0-8 en los 8 canales, que es lo esperable sin nada que refleje. Falta confirmar que sube sobre blanco (ver §5).
- Motores y PID en pista: nunca se ha corrido sobre una línea.
- Conexión completa app ↔ robot con este firmware.

### App (rama `app-v2`, versión 2.0.0+2)
- Inicio con 3 secciones. El joystick solo envía mientras su pantalla está abierta y al salir manda alto.
- Seguidor:
  - Barras de los 8 sensores con escala automática.
  - Raya de umbral que se arrastra, y marcador de posición.
  - Kp, Ki, Kd y velocidad con +/- o escribiendo el valor.
  - Leer y enviar parámetros, color de línea, calibrar o terminar calibración, iniciar y detener.
- Terminal: historial de 500 líneas, comandos rápidos, oculta las lecturas `SEN` por defecto.

**Probado:** `flutter test` (19 pruebas del protocolo) pasa, y `flutter analyze` no da errores (los 39 avisos ya existían). El APK de release compila.

**Sin probar:** conectado a un robot real.

---

## 4. Hardware: lo que hay que saber

- **Conector del sensor (CN1): 1 GND, 2 3.3 V, 3 SCL, 4 SDA.** El 2026-11-06 el sensor "no se detectaba" porque el **cable estaba invertido**: dejaba SDA y SCL en 0 V y tumbaba todo el bus I2C, OLED incluida. También conviene revisar que el ESP32 esté bien asentado en el zócalo de la shield. Para diagnosticar, GPIO21 y GPIO22 deben leer 1 con pull-up y el escaneo I2C debe mostrar 0x3C y 0x48.
- **Señal débil:** el ADS7830 usa su referencia interna de 2.5 V. Sin calibrar, el blanco puede leerse bajo (estimado 50-70 por el circuito, sin medir). **Hay que calibrar** antes de usar el umbral de 128.
- **Motores:** izquierdo en el puerto 4 y derecho en el 5 (robot de 4 motores: además el 1 y el 3). La librería recibe el ciclo de trabajo en % (0-100) y el sketch convierte desde 0-255 en `Drive()`.
- **Placa por USB:** aparece como puerto COM de un CH340, y el número cambia al reconectar (fue COM5 y luego COM3).
- **Sensor montado al revés:** si S0 queda a la derecha del robot, poner `INVERTIR_SENSORES true`.

---

## 5. Siguientes pasos sugeridos (en orden)

1. **Fusionar cuando se pruebe en el robot:** `seguidor-de-linea` en `main` de la librería, subiendo la versión en `library.properties`. Quien la instala desde el Library Manager recibe la 2.0.1, sin las correcciones de BLE. En la app, `app-v2` en `main` cuando se vaya a publicar.
2. **Medir el sensor sobre superficies reales:** papel blanco y cinta negra, a 2-3 mm de altura, en el Serial Plotter. Si el blanco no pasa de unos 20, revisar si los LEDs IR encienden (se ven morados con la cámara del celular). El cable invertido pudo dañar el ADS7830 o los LEDs.
3. **Prueba app ↔ robot:** conectar, ver las barras, `GET` (deben llegar PID, umbral y color), calibrar, cambiar el umbral y el color, iniciar y detener, y usar la Terminal.
4. **En pista:** confirmar el sentido de los motores (signos en `motores()`) y el orden de los sensores, y afinar el PID con los pasos finos de la app.
5. **Limitaciones conocidas que el dueño decidió posponer** (detalle en `PROTOCOLO_APP.md` §8):
   - El PID, el umbral, la calibración y el color **no se guardan en el robot** (se podrían guardar con `Preferences`). Tras un reinicio, la app recibe los valores de fábrica y pisa los afinados.
   - Los valores por defecto de la app (0.05 / 0 / 0.5 / 150) no coinciden con los del robot (0.0128 / 0 / 0.089 / 128).
   - Los mensajes de texto del robot solo se ven en la Terminal, no en la pantalla del seguidor.
6. **Otros pendientes de la app:**
   - Para Play Store hace falta la keystore real (hoy se firma con la clave de debug) y subir `targetSdk` de 34 a 35.
   - El plugin `app_settings` 9.0.1 avisa que una versión futura de Flutter dejará de compilarlo.
   - La librería satura los motores por encima de 100 en sketches que no convierten a %, como `control_remoto_robot`.

---

## 6. Cómo compilar y cargar

**Firmware**
- ESP32 core `esp32:esp32` 3.3.x. Librerías: Adafruit SSD1306, Adafruit GFX, Adafruit VL53L0X, Adafruit PCF8574 (dependencias de ROBLEX).
- La librería ROBLEX debe ser el clon del repo, no la del Library Manager. En el PC del dueño, `Documents\Arduino\libraries\ROBLEX` es un junction al clon.
- `arduino-cli compile --fqbn esp32:esp32:esp32 --upload -p COMx <carpeta sensor_de_linea>`, o el Arduino IDE con placa *ESP32 Dev Module*. Compilar tarda unos 5 minutos.

**App**
- Flutter 3.47 (stable), JDK 21 (`flutter config --jdk-dir`), Android SDK 36 y NDK 28.2. Gradle 9.3.1, AGP 9.1.1 y Kotlin 2.4.0 ya están configurados en el proyecto.
- `git switch app-v2`, `flutter test`, `flutter build apk --release`.

---

## 7. Historial breve

| Fecha | Qué |
|---|---|
| 2026-06 | App migrada a BLE (`code`); librería ROBLEX 2.0 con BLE |
| 2026-10-19/20 | Migración de build de la app; branch `app-v2` con inicio, seguidor y terminal; protocolo definido |
| 2026-10-20 | Librería: MTU 517 y cola de recepción. Primer sketch del sensor ADS7830 con BLE, que luego se volvió el seguidor completo |
| 2026-10-21/22 | Umbral (`UMB`), color de línea (`LIN`), modo calibración (`CAL,1/0`), barras con escala automática |
| 2026-11-06 | Prueba en la placa real: el cable del sensor estaba invertido; corregido, el sensor responde en 0x48 |
| 2026-11-12 | Todo subido a GitHub (librería `seguidor-de-linea`, app `app-v2` y `code`); documentación en `extras/` |
