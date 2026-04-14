#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

uint8_t deviceAddress = 0x01;
long baudRate = 9600;

float LUX = 0;

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
void buildFrame(uint8_t addr, uint8_t func, uint16_t reg, uint16_t num, uint8_t *frame) {
  frame[0] = addr;
  frame[1] = func;
  frame[2] = reg >> 8;
  frame[3] = reg & 0xFF;
  frame[4] = num >> 8;
  frame[5] = num & 0xFF;

  uint16_t crc = calcCRC(frame, 6);

  // Modbus: low byte trước
  frame[6] = crc & 0xFF;
  frame[7] = crc >> 8;
}

// ================= Read N bytes =================
uint8_t readN(uint8_t *buf, size_t len) {
  size_t count = 0;
  long start = millis();

  while (count < len) {
    if (mySerial.available()) {
      buf[count++] = mySerial.read();
    }
    if (millis() - start > 500) break;
  }
  return count;
}

// ================= Read Lux =================
bool readLux() {
  uint8_t frame[8];
  uint8_t data[9];

  buildFrame(deviceAddress, 0x03, 0x0002, 0x0002, frame);

  mySerial.write(frame, 8);
  delay(20);

  if (readN(data, 9) == 9) {
    uint16_t crc = calcCRC(data, 7);
    uint16_t crcRecv = (data[8] << 8) | data[7];

    if (crc == crcRecv) {
      LUX = (uint32_t)data[3] << 24 |
            (uint32_t)data[4] << 16 |
            (uint32_t)data[5] << 8  |
            (uint32_t)data[6];

      LUX /= 1000.0;
      return true;
    }
  }
  return false;
}

/* ================= Read Version =================
void readVersion() {
  uint8_t frame[8];
  uint8_t data[9];

  // Theo tài liệu: register version = 0x0064
  buildFrame(deviceAddress, 0x03, 0x0064, 0x0002, frame);

  mySerial.write(frame, 8);
  delay(20);

  if (readN(data, 9) == 9) {
    uint16_t crc = calcCRC(data, 7);
    uint16_t crcRecv = (data[8] << 8) | data[7];

    if (crc == crcRecv) {
      uint8_t major = data[3];
      uint8_t minor = data[4];

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

// ================= Setup =================
void setup() {
  Serial.begin(9600);

  Serial.println("Nhap dia chi (hex, vd: 01, 02):");
  while (!Serial.available());
  deviceAddress = strtol(Serial.readString().c_str(), NULL, 16);

  Serial.print("Address set = 0x");
  Serial.println(deviceAddress, HEX);

  Serial.println("Nhap baudrate (vd: 9600, 4800):");
  while (!Serial.available());
  baudRate = Serial.parseInt();

  Serial.print("Baudrate set = ");
  Serial.println(baudRate);

  mySerial.begin(baudRate);
}

// ================= Loop =================
void loop() {

  if (readLux()) {
    Serial.print("Lux = ");
    Serial.println(LUX, 3);
  } else {
    Serial.println("Read Lux failed");
  }

  delay(1000);

  //readVersion();

  //delay(2000);
}