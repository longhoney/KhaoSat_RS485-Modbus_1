#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

long baudList[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600};

uint8_t frame[8];
uint8_t response[9];

uint8_t foundAddress = 0;
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

// ================= Build frame =================
void buildFrame(uint8_t addr, uint16_t reg, uint8_t *frm) {
  frm[0] = addr;
  frm[1] = 0x03;
  frm[2] = reg >> 8;
  frm[3] = reg & 0xFF;
  frm[4] = 0x00;
  frm[5] = 0x02;

  uint16_t crc = calcCRC(frm, 6);
  frm[6] = crc & 0xFF;
  frm[7] = crc >> 8;
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

// ================= Scan =================
bool scanDevice() {
  Serial.println("Scanning...");

  for (int i = 0; i < 7; i++) {
    long baud = baudList[i];

    Serial.print("Try baud: ");
    Serial.println(baud);

    mySerial.end();
    delay(100);
    mySerial.begin(baud);
    delay(200);

    while (mySerial.available()) mySerial.read();

    // Broadcast query config
    buildFrame(0xFF, 0x0064, frame);

    mySerial.write(frame, 8);
    delay(50);

    if (readN(response, 9) == 9) {

      uint16_t crcCalc = calcCRC(response, 7);
      uint16_t crcRecv = (response[8] << 8) | response[7];

      if (crcCalc == crcRecv) {

        foundAddress = (response[3] << 8) | response[4];
        uint16_t baudCode = (response[5] << 8) | response[6];

        foundBaud = baud;

        Serial.println("=== FOUND DEVICE ===");
        Serial.print("Address: ");
        Serial.println(foundAddress);

        Serial.print("Baudrate: ");
        Serial.println(foundBaud);

        return true;
      }
    }

    Serial.println("No response");
  }

  Serial.println("Scan failed!");
  return false;
}

/* ================= Read Version =================
void readVersion() {
  Serial.println("\nReading Version...");

  buildFrame(foundAddress, 0x0064, frame);

  mySerial.write(frame, 8);
  delay(50);

  if (readN(response, 9) == 9) {

    uint16_t crcCalc = calcCRC(response, 7);
    uint16_t crcRecv = (response[8] << 8) | response[7];

    if (crcCalc == crcRecv) {
      uint8_t major = response[3];
      uint8_t minor = response[4];

      Serial.print("Version: ");
      Serial.print(major);
      Serial.print(".");
      Serial.println(minor);
    } else {
      Serial.println("CRC error (version)");
    }
  } else {
    Serial.println("No response (version)");
  }
}*/

// ================= Read Speed =================
void readSpeed() {
  Serial.println("\nReading Speed...");

  buildFrame(foundAddress, 0x0046, frame);

  mySerial.write(frame, 8);
  delay(50);

  if (readN(response, 9) == 9) {

    uint16_t crcCalc = calcCRC(response, 7);
    uint16_t crcRecv = (response[8] << 8) | response[7];

    if (crcCalc == crcRecv) {

      uint16_t speed = (response[3] << 8) | response[4];

      Serial.print("Speed value: ");
      Serial.println(speed);

      Serial.println("(Don vi phu thuoc vao datasheet - thuong la ms)");
    } else {
      Serial.println("CRC error (speed)");
    }
  } else {
    Serial.println("No response (speed)");
  }
}

// ================= Setup =================
void setup() {
  Serial.begin(9600);
  delay(2000);

  if (scanDevice()) {
    delay(500);

    // Set lại đúng baud tìm được
    mySerial.begin(foundBaud);

    //readVersion();
    readSpeed();
  }
}

void loop() {
}