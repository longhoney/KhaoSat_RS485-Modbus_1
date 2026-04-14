#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

int myBaud = 9600;

// Frame gửi: 12 byte + 2 CRC
uint8_t frame[14];

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
void buildFrame() {
  uint8_t temp[12] = {
    0x01, // địa chỉ hiện tại
    0x10, // function
    0x00, 0x64, // register start
    0x00, 0x03, // số register
    0x00, 0x21, // address mới = 0x21
    0x00, 0x02, // baudrate = 4800
    0x00, 0x00  // reserved
  };

  // copy vào frame
  for (int i = 0; i < 12; i++) {
    frame[i] = temp[i];
  }

  // tính CRC
  uint16_t crc = calcCRC(frame, 12);
  frame[13] = crc & 0xFF;
  frame[14] = crc >> 8;
}

// ================= Read =================
int readResponse(uint8_t *buf, int maxLen) {
  int count = 0;
  long start = millis();

  while (millis() - start < 500) {
    while (mySerial.available()) {
      if (count < maxLen) {
        buf[count++] = mySerial.read();
      }
    }
  }
  return count;
}

// ================= Print HEX =================
void printHex(uint8_t *data, int len) {
  Serial.println("----- RESPONSE (HEX) -----");

  for (int i = 0; i < len; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  Serial.println();
  Serial.println("--------------------------");
}

// ================= Setup =================
void setup() {
  Serial.begin(9600);
  mySerial.begin(myBaud);

  buildFrame();

  Serial.println("=== SEND CONFIG COMMAND ===");
}

// ================= Loop =================
void loop() {
  uint8_t buffer[32];

  // gửi 1 lần duy nhất
  mySerial.write(frame, 15);

  delay(100);

  int len = readResponse(buffer, sizeof(buffer));

  if (len > 0) {
    printHex(buffer, len);

    Serial.println(">>> Cấu hình thành công!");
    Serial.println("Địa chỉ mới: 0x21");
    Serial.println("Baudrate mới: 4800");

    Serial.println("!!! Vui lòng NGẮT NGUỒN cảm biến và cấp lại !!!");

  } else {
    Serial.println("No response");
  }

  // chỉ chạy 1 lần
  while (1);
}