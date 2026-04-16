/**
 * This is a simple demo code that reads the Temperature and Humidity of the "SHT RS485 Modbus RTU".
 * With default sensor configuration parameters: Baudrate 4800 bit/s ; Address 0x01.
 *
 * There are 2 ways to use it:
 * 1. Use the Struct variable "dataSHT"
 * 2. Read each value type separately
 */

// Add the library
#include "SHTModbus.h"
#include <MKE_LiquidCrystal_I2C.h>


/* ------------------------------------------------------------------------- */

#define RX_SHT 2
#define TX_SHT 3
/* Using with SoftwareSerial, default (RX=2) and (TX=3) */
SHT demo(RX_SHT, TX_SHT, BAUD_4800, 0x01);

/* Using with HardwareSerial, default (RX=0) and (TX=1) */
// SHT demo(BAUD_4800, 0x01);
/* ------------------------------------------------------------------------- */

// LCD config
MKE_LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  /* ----------------------- Test with SoftwareSerial ---------------------- */
  Serial.begin(115200);
  Serial.println(F("Start reading SHT Modbus (HSHOP)"));

  demo.begin(4800);

  /* ----------------------- Test with HardwareSerial ---------------------- */
  // Serial.begin(4800);
  // Serial.println(F("Start reading SHT Modbus (HSHOP)"));

  // demo.begin(4800);

  /* ----------------------- Initialize LCD ----------------------- */
  lcd.init();
  lcd.backlight();

  /* ----------------------- Display a welcome message ----------------------- */
  lcd.setCursor(0, 0); lcd.print("Welcome to");
  lcd.setCursor(0, 1); lcd.print("SHT40-IP67-RS485");
  delay(3000);
  lcd.clear();
}

void loop() {
  /* --------------------------------- [01] -------------------------------- */
  dataSHT data = demo.getData();

  /* In ket qua len Serial Monitor: 27.40°C [7 ky tu] 81.32°F 55.40%
  Serial.print(data.temperatureC);
  Serial.print(F("°C\t"));
  Serial.print(data.temperatureF);
  Serial.print(F("°F\t"));
  Serial.print(data.humidity);
  Serial.println(F("%"));
  */
  /* ----------------------- IN ra LCD nhiet do C va do am % ----------------------- */
  lcd.setCursor(0, 0); lcd.print("SHT40-IP67-RS485");
  lcd.setCursor(1, 1);
  lcd.print(data.temperatureC); lcd.print("oC\t");
  lcd.print(data.humidity);
  lcd.print("%");

  delay(1000);
}
