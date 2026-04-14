#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

long baudList[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600};

uint8_t frame[8];
uint8_t response[9];

long foundBaud = 0;

// ================= CRC =================
uint16_t calcCRC(uint8_t *data, uint8_t len) {
  uint16_t crc = 0xFFFF;
  for (int i = 0; i < len; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x0001)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc >>= 1;
    }
  }
  return crc;
}

// ================= Read =================
uint8_t readN(uint8_t *buf, size_t len) {
  size_t count = 0;
  long start = millis();

  while (count < len) {
    if (mySerial.available()) {
      buf[count++] = mySerial.read();
    }
    if (millis() - start > 300) break;
  }
  return count;
}

// ================= Scan baud =================
bool scanBaud() {
  Serial.println("Scanning baudrate...");

  for (int i = 0; i < 7; i++) {

    long baud = baudList[i];

    Serial.print("Try: ");
    Serial.println(baud);

    mySerial.end();
    delay(100);
    mySerial.begin(baud);
    delay(200);

    while (mySerial.available()) mySerial.read();

    // Query config: FF 03 00 64 00 02
    frame[0] = 0xFF;
    frame[1] = 0x03;
    frame[2] = 0x00;
    frame[3] = 0x64;
    frame[4] = 0x00;
    frame[5] = 0x02;

    uint16_t crc = calcCRC(frame, 6);
    frame[6] = crc & 0xFF;
    frame[7] = crc >> 8;

    mySerial.write(frame, 8);
    delay(50);

    if (readN(response, 9) == 9) {

      uint16_t crcCalc = calcCRC(response, 7);
      uint16_t crcRecv = (response[8] << 8) | response[7];

      if (crcCalc == crcRecv) {
        foundBaud = baud;

        Serial.println(">>> FOUND BAUD <<<");
        Serial.println(foundBaud);
        return true;
      }
    }

    Serial.println("No response");
  }

  Serial.println("Scan failed!");
  return false;
}

// ================= Factory reset =================
void factoryReset() {

  // Frame: FF 06 00 F0 00 00 + CRC
  frame[0] = 0xFF;
  frame[1] = 0x06;
  frame[2] = 0x00;
  frame[3] = 0xF0;
  frame[4] = 0x00;
  frame[5] = 0x00;

  uint16_t crc = calcCRC(frame, 6);
  frame[6] = crc & 0xFF;
  frame[7] = crc >> 8;

  Serial.println("\nSending factory reset...");
  mySerial.write(frame, 8);

  delay(200);

  Serial.println("\n=== DONE ===");
  Serial.println("Vui long tat nguon cam bien!");
  Serial.println("Cap nguon lai de hoan tat reset.");

  Serial.println("\nSau reset:");
  Serial.println("- Address = 0x01");
  Serial.println("- Baudrate = 9600");
}

// ================= Setup =================
void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.println("=== FACTORY RESET TOOL ===");
  Serial.println("Nhan phim bat ky de bat dau...");

  while (!Serial.available());

  if (scanBaud()) {

    mySerial.begin(foundBaud);

    factoryReset();

  } else {
    Serial.println("Khong tim thay thiet bi!");
  }
}

void loop() {
}