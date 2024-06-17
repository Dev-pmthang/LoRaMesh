#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include "gpio.h"
#include "readsensor.h"

#define NSS 5
#define RST 14
#define DI0 27

const uint16_t readingID = 2;
bool shouldSendData = true; // Cờ kiểm soát việc gửi dữ liệu

String data = "";

String dataSensor;
String sendnode2;

unsigned long previousMillis = 0; // Thời gian cho khoảng đọc cảm biến
const long sensorInterval = 2000; // Khoảng thời gian đọc cảm biến (điều chỉnh theo nhu cầu)

void LoRa_txMode()
{ // Thiết lập chế độ truyền
  LoRa.idle();
  LoRa.disableInvertIQ();
}

void LoRa_rxMode()
{ // Thiết lập chế độ nhận
  LoRa.enableInvertIQ();
  LoRa.receive();
}

void LoRa_sendMessage(String message)
{
  LoRa_txMode();
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket(true);
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
      if (LoRaData == "N2")
      {
        String sendnode2 = String(readingID) + "/" + dataSensor;
        LoRa_sendMessage(sendnode2);
      }
      else if (LoRaData == "N3")
      {
        String sendnode3 = "N#";
        LoRa_sendMessage(sendnode3);
      }
      else if (LoRaData == "N@")
      {
        String node3 = "3/" + data;
        LoRa_sendMessage(node3);
      }
    }
  }
  LoRa.receive();
}

void lora_init()
{
  Serial.println("LoRa NODE 2");
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
  init_gpio();
  lora_init();
  dataSensor = Read_data();
  LoRa.onReceive(onReceive);
  LoRa.receive(); // Sẵn sàng nhận dữ liệu
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
