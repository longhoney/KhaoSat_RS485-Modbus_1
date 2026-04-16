#include "MKE_DHT.h"
// Đặt tên cho chân kết nối cảm biến
#define DHTPIN 10     // Digital pin connected to the DHT sensor

MKE_DHT dht(DHTPIN, DHT11);

// Tạo biến lưu giá trị cảm biến
// value read from the sensor
float sensorValue1 = 0;
float sensorValue2 = 0;

void setup()
{
  // Khởi động kết nối Serial UART ở tốc độ 115200 để truyền dữ liệu lên máy tính.
  // Start the Serial UART connection at 115200 to transfer data to the computer.
  Serial.begin(115200);
  dht.begin();
}

void loop()
{
  // Đọc giá trị cảm biến
  // Get value
  sensorValue1 = dht.readTemperature();
  sensorValue2 = dht.readHumidity();

  // Hiển thị giá trị đo được của cảm biến lên máy tính.
  // Show the sensor value on Arduno Serial Monitor
  Serial.print("Temp: ");
  Serial.print(sensorValue1);
  Serial.print("  Humi: ");
  Serial.println(sensorValue2);
  
  // Chờ 2000ms
  // Wait 2000ms
  delay(2000);
}