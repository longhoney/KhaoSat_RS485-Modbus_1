/*
  Gui lenh thay doi dia chi va baudrate
  In ra ket qua da thay doi
  In ra nhac nho tat nguon cam bien
*/

#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

int myBaud = 9600;    //Nhap baudrate cua thiet bi
uint8_t frame[14];

// ================= Build frame =================
void buildFrame() {
  frame[0] = 0x01;  // current Address 
  frame[1] = 0x10;  // Function code
  frame[2] = 0x00;  // Register start address 1
  frame[3] = 0x64;  // Register start address 2
  frame[4] = 0x00;  // Register length 1
  frame[5] = 0x03;  // Register length 2
  frame[6] = 0x00;  // Modify address 1
  frame[7] = 0x21;  // Modify address 2 = Nhap dia chi moi tai day
  // Baudrate on dinh: 0-1200, 1-2400, 2-4800, 3-9600. Baudrate con lai: 4-19200, 5-38400, 6-57600
  frame[8] = 0x00;  // Modify baud rate 1
  frame[9] = 0x02;  // Modify baud rate 2 = Nhap code baudrate moi tai day
  frame[10] = 0x00; // Parity check 1
  frame[11] = 0x00; // Parity check 2
  uint16_t crc = calcCRC(frame, 12);
  frame[12] = crc & 0xFF;
  frame[13] = crc >> 8;
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
  delay(3000);

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
}

// ================= Loop =================
void loop() {
  /*
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
  */
}