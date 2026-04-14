/* Gui lenh doc data dang Hex (0x01, 9600)
  Nhan data dang HEX
*/

#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

int myBaud = 9600; //Nhap baudrate cua cam bien
uint8_t frame[8];  //Do dai frame du lieu gui di - nhan ve: 8 byte

// ================= Build frame =================
void buildFrame() {
  frame[0] = 0x01; // địa chỉ (có thể đổi)
  frame[1] = 0x03;
  frame[2] = 0x00;
  frame[3] = 0x02;
  frame[4] = 0x00;
  frame[5] = 0x02;

  uint16_t crc = calcCRC(frame, 6);
  frame[6] = crc & 0xFF;
  frame[7] = crc >> 8;
}

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

// ================= Read all =================
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
    if (data[i] < 0x10) Serial.print("0"); // thêm số 0 phía trước
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

  Serial.println("Sending frame...");
}

// ================= Loop =================
void loop() {

  uint8_t buffer[32];

  // gửi lệnh
  mySerial.write(frame, 8);

  delay(50);

  // đọc response
  int len = readResponse(buffer, sizeof(buffer));

  if (len > 0) {
    printHex(buffer, len);
  } else {
    Serial.println("No response");
  }

  delay(2000);
}