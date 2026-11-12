#include "ROBLEX.h"

// Modulo sensor de linea: 8 sensores QRE1113 conectados a un ADS7830 (ADC de
// 8 bits y 8 canales por I2C). Cada fototransistor sube el voltaje de su canal
// cuanto mas luz refleja la superficie: blanco = valor alto, negro = valor bajo.
// El modulo no tiene referencia externa, asi que el ADC usa la interna de 2.5V:
// cualquier voltaje por encima de 2.5V se lee como 255.

// byte de comando del ADS7830: SD C2 C1 C0 PD1 PD0 X X
#define ADS7830_SINGLE  0x80  // SD = 1: cada canal contra COM (GND)
#define ADS7830_REF_ON  0x0C  // PD = 11: referencia interna y ADC siempre encendidos

static uint8_t lineAddress = 0x48;

bool ROBLEX::SetupLine(uint8_t address) {
  lineAddress = address;

  Wire.beginTransmission(lineAddress);
  if (Wire.endTransmission() != 0) {
    return false;  // nadie respondio en esa direccion
  }

  LineValue(0);  // la primera conversion enciende la referencia interna
  delay(10);     // esperar a que la referencia se estabilice
  return true;
}

int ROBLEX::LineValue(uint8_t sensor) {
  if (sensor > 7) {
    return -1;
  }

  // en modo simple los canales van intercalados: C2 = bit 0 del canal, C1 C0 = canal / 2
  uint8_t command = ADS7830_SINGLE | ((sensor & 1) << 6) | ((sensor >> 1) << 4) | ADS7830_REF_ON;

  // escribir el comando y leer el resultado en una sola transaccion (repeated start):
  // la mitad de transacciones I2C, importante para leer rapido en un seguidor de linea
  Wire.beginTransmission(lineAddress);
  Wire.write(command);
  if (Wire.endTransmission(false) != 0) {
    return -1;
  }
  if (Wire.requestFrom(lineAddress, (uint8_t)1) != 1) {
    return -1;  // el modulo no respondio
  }
  return Wire.read();
}

bool ROBLEX::LineValues(int *values) {
  for (uint8_t i = 0; i < 8; i++) {
    int value = LineValue(i);
    if (value < 0) {
      return false;
    }
    values[i] = value;
  }
  return true;
}
