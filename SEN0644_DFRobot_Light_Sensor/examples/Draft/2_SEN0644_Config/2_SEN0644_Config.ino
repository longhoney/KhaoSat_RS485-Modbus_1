#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

uint8_t foundAddress = 0;
long foundBaud = 0;

uint8_t frame[8];
uint8_t response[9];

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

// ================= Scan =================
bool scanDevice() {
  long baudList[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600};

  for (int i = 0; i < 7; i++) {
    mySerial.end();
    delay(100);
    mySerial.begin(baudList[i]);
    delay(200);

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
        foundAddress = response[4];
        foundBaud = baudList[i];

        Serial.println("=== FOUND DEVICE ===");
        Serial.print("Address: ");
        Serial.println(foundAddress);
        Serial.print("Baudrate: ");
        Serial.println(foundBaud);
        return true;
      }
    }
  }
  return false;
}

// ================= Baud code → text =================
long codeToBaud(uint8_t code) {
  switch (code) {
    case 0: return 1200;
    case 1: return 2400;
    case 2: return 4800;
    case 3: return 9600;
    case 4: return 19200;
    case 5: return 38400;
    case 6: return 57600;
    default: return 9600;
  }
}

// ================= Read input string =================
String readLine() {
  while (!Serial.available()); // CHỜ người dùng nhập
  String input = Serial.readStringUntil('\n');
  input.trim(); // bỏ \r\n
  return input;
}

// ================= Write config =================
void writeConfig(uint8_t newAddr, uint8_t baudCode) {

  // Write address
  frame[0] = foundAddress;
  frame[1] = 0x06;
  frame[2] = 0x00;
  frame[3] = 0x64;
  frame[4] = 0x00;
  frame[5] = newAddr;

  uint16_t crc = calcCRC(frame, 6);
  frame[6] = crc & 0xFF;
  frame[7] = crc >> 8;

  Serial.println("\nWriting new address...");
  mySerial.write(frame, 8);
  delay(100);

  // Write baudrate
  frame[0] = newAddr;
  frame[1] = 0x06;
  frame[2] = 0x00;
  frame[3] = 0x65;
  frame[4] = 0x00;
  frame[5] = baudCode;

  crc = calcCRC(frame, 6);
  frame[6] = crc & 0xFF;
  frame[7] = crc >> 8;

  Serial.println("Writing new baudrate...");
  mySerial.write(frame, 8);
  delay(100);

  Serial.println("\n=== CONFIG UPDATED ===");
  Serial.print("New Address: ");
  Serial.println(newAddr);

  Serial.print("New Baudrate: ");
  Serial.println(codeToBaud(baudCode));

  Serial.println("\n!!! PLEASE POWER OFF DEVICE AND TURN ON AGAIN !!!");
}

// ================= Setup =================
void setup() {
  Serial.begin(9600);
  delay(2000);

  if (scanDevice()) {

    Serial.println("\nNhap dia chi moi (1-247) roi nhan ENTER:");
    String addrStr = readLine();
    uint8_t newAddr = addrStr.toInt();

    Serial.println("\nNhap baudrate code roi nhan ENTER:");
    Serial.println("0 = 1200");
    Serial.println("1 = 2400");
    Serial.println("2 = 4800");
    Serial.println("3 = 9600");
    Serial.println("4 = 19200");
    Serial.println("5 = 38400");
    Serial.println("6 = 57600");

    String baudStr = readLine();
    uint8_t baudCode = baudStr.toInt();

    // CHỈ chạy khi đã nhập đủ
    writeConfig(newAddr, baudCode);

  } else {
    Serial.println("Device not found!");
  }
}

void loop() {
}