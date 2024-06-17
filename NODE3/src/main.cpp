#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include "SHT3x.h"

#define NSS 5
#define RST 14
#define DI0 27

SHT3x Sensor;
const uint16_t readingID = 3;
bool shouldSendData = true;

String data = "";
String temp;
String humi;
String dataSensor;
String sendnode3;

unsigned long previousMillis = 0; // Thời gian cho khoảng đọc cảm biến
const long sensorInterval = 2000; // Khoảng thời gian đọc cảm biến (điều chỉnh theo nhu cầu)

unsigned long transmitMillis = 0;   // Thời gian cho khoảng truyền dữ liệu
const long transmitInterval = 5000; // Khoảng thời gian truyền dữ liệu (điều chỉnh theo nhu cầu)

// Định nghĩa chân ADC mà cảm biến độ ẩm đất được kết nối
const int soilMoisturePin = 4;

// Phạm vi giá trị ADC của cảm biến độ ẩm đất
const int minValue = 0;    // Giá trị ADC tương ứng với độ ẩm đất tối thiểu
const int maxValue = 4095; // Giá trị ADC tương ứng với độ ẩm đất tối đa

// Phạm vi giá trị độ ẩm đất bạn mong muốn, ví dụ từ 0% đến 100%
const int minMoisture = 0;   // Độ ẩm đất tối thiểu
const int maxMoisture = 100; // Độ ẩm đất tối đa

// Hàm ánh xạ dữ liệu từ phạm vi ADC sang phạm vi độ ẩm đất mong muốn
int mapMoisture(int adcValue)
{
  return (100 - map(adcValue, minValue, maxValue, minMoisture, maxMoisture));
}

// Hàm đọc giá trị từ cảm biến độ ẩm đất và ánh xạ dữ liệu
int readSoilMoisture()
{
  int adcValue = analogRead(soilMoisturePin);
  return mapMoisture(adcValue);
}

void LoRa_rxMode()
{                        // nhan du lieu
  LoRa.enableInvertIQ(); // active invert I and Q signals
  LoRa.receive();        // set receive mode
}

void LoRa_txMode()
{                         // truyen du lieu
  LoRa.idle();            // set standby mode
  LoRa.disableInvertIQ(); // normal mode
}

void LoRa_sendMessage(String message)
{
  LoRa_txMode();        // set tx mode
  LoRa.beginPacket();   // start packet
  LoRa.print(message);  // add payload
  LoRa.endPacket(true); // finish packet and send it
}

String Read_data()
{
  Sensor.UpdateData();
  if ((Sensor.GetTemperature() != 0.00) && (Sensor.GetRelHumidity() != 0.00))
  {
    int moisture = readSoilMoisture();
    String soilMoistureValue = String(moisture);
    String temp = String(Sensor.GetTemperature());
    String humi = String(Sensor.GetAbsHumidity());

    String DATA_SENSOR = temp + "@" + humi + "*" + soilMoistureValue;
    return DATA_SENSOR;
  }
  else
  {
    return String("ERROR SHT30"); // Error message if SHT3x sensor data unavailable
  }
}

void onReceive(int packetSize)
{
  if (packetSize == 0)
    return;
  if (packetSize)
  {
    while (LoRa.available())
    {
      String LoRaData = LoRa.readString();
      if (LoRaData == "N#")
      {
        String sendnode3 = "3/" + dataSensor;
        LoRa_sendMessage(sendnode3);
      }
    }
  }
  LoRa.receive(); // Sẵn sàng nhận dữ liệu
}

void lora_init()
{
  Serial.println("LoRa NODE 3");
  LoRa.setPins(NSS, RST, DI0);
  if (!LoRa.begin(433E6))
  {
    Serial.println("LoRa initialization failed!");
    while (1)
      ;
  }
  LoRa.setTxPower(13);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();
  LoRa.setSyncWord(0xF2);
  Serial.println("LoRa Node initialization successful!");
}

void setup()
{
  Serial.begin(115200);
  lora_init();
  pinMode(soilMoisturePin, INPUT);
  dataSensor = Read_data();
  LoRa.onReceive(onReceive);
  LoRa.receive();
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= sensorInterval)
  {
    previousMillis = currentMillis;
    dataSensor = Read_data();
    Serial.println(dataSensor);
  }
}