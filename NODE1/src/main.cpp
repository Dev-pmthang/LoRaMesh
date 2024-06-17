#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "SHT3x.h"
#include "EmonLib.h"

#define NSS 5
#define RST 14
#define DI0 27
#define LED1 32
#define SSR 26
#define ADC_INPUT 36

#define Dv_ON 1
#define Dv_OFF 0
bool thuchien;
char on_off;

EnergyMonitor emon1;
SoftwareSerial mySerial(16, 17); // RX, TX
SHT3x Sensor;

// CO2 sensor communication command (modify if needed based on datasheet)
unsigned char hexData[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

uint16_t readingID = 1; // Use uint16_t or int16_t if appropriate
String dataSensor;
String data;
String temp;
String humi;
unsigned long previousMillis = 0; // Biến lưu thời gian trước đó
const long interval = 7000;       // không gian lưu trữ giữa các lần gửi
int LEDState;

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
  return map(adcValue, minValue, maxValue, minMoisture, maxMoisture);
}

// Hàm đọc giá trị từ cảm biến độ ẩm đất và ánh xạ dữ liệu
int readSoilMoisture()
{
  int adcValue = analogRead(soilMoisturePin);
  return mapMoisture(adcValue);
}

void LoRa_txMode() // truyen du lieu
{
  LoRa.idle();            // set standby mode
  LoRa.disableInvertIQ(); // normal mode
}

void LoRa_rxMode()
{                        // nhan du lieu
  LoRa.enableInvertIQ(); // active invert I and Q signals
  LoRa.receive();        // set receive mode
}

void LoRa_sendMessage(String message)
{
  LoRa_txMode();        // set tx mode
  LoRa.beginPacket();   // start packet
  LoRa.print(message);  // add payload
  LoRa.endPacket(true); // finish packet and send it
}

void LoRa_init()
{
  Serial.println("LoRa NODE1");
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

String Data_CO2()
{
  mySerial.write(hexData, 9); // Send command to request CO2 data
  delay(500);

  for (int i = 0, j = 0; i < 9; i++)
  {
    if (mySerial.available() > 0)
    {
      long hi, lo, CO2;
      int ch = mySerial.read();

      if (i == 2)
      {
        hi = ch; // Read high byte (assuming high byte first) - adjust based on datasheet
      }
      else if (i == 3)
      {
        lo = ch; // Read low byte (assuming low byte second) - adjust based on datasheet
      }
      else if (i == 8)
      {
        CO2 = hi * 256 + lo; // Combine bytes into CO2 value
        return String(CO2);
      }
    }
    else
    {
      return String("ERROR CO2"); // Error message if no data received
      break;
    }
  }
  return String("0"); // Return 0 if no valid data received
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

    String DATA_SENSOR = temp + "@" + humi + "*" + soilMoistureValue + "#" + Data_CO2();
    return DATA_SENSOR;
  }
  else
  {
    return String("ERROR SHT30"); // Error message if SHT3x sensor data unavailable
  }
}

String feedbackoff()
{
  double Irms = emon1.calcIrms(1480); // Calculate Irms only
  if (Irms < 2)
  {
    digitalWrite(LED1, 1);
  }
  return String("SSR_OFF"); // Trả về trạng thái của LED
}

String feedbackon()
{
  double Irms = emon1.calcIrms(1480); // Calculate Irms only
  if (Irms > 2)
  {
    digitalWrite(LED1, 0);
  }
  return String("SSR_ON"); // Trả về trạng thái của LED
}

void ON_SSR()
{                       // Calculate Irms only
  digitalWrite(SSR, 1); // Turn on LED1 if Irms > 2, else turn it off
}
void OFF_SSR()
{
  digitalWrite(SSR, 0);
}

void onReceive(int packetSize)
{
  if (packetSize == 0)
    return;
  if (packetSize)
  {
    while (LoRa.available())
    {
      String data = LoRa.readString();
      if (data == "N1")
      {
        String sendnode1 = String(readingID) + "/" + dataSensor;
        LoRa_sendMessage(sendnode1);
      }
      else if (data == "N1ON")
      {
        // ON_SSR();
        // double Irms = emon1.calcIrms(1480); // Calculate Irms only
        // if (Irms > 2)
        // {
        //   digitalWrite(LED1, 0);
        // }
        // LoRa_sendMessage("ON_SSR");
        on_off = 1;
        thuchien = 1;
      }
      else if (data == "N1OFF")
      {
        on_off = 0;
        thuchien = 1;
        // OFF_SSR();
        // double Irms = emon1.calcIrms(1480); // Calculate Irms only
        // if (Irms < 2)
        // {
        //   digitalWrite(LED1, 1);
        // }
        // LoRa_sendMessage("OFF_SSR");
      }
    }
  }
  LoRa.receive(); // Sẵn sàng nhận dữ liệu
}

void setup()
{
  Serial.begin(115200);
  mySerial.begin(9600);
  analogReadResolution(ADC_BITS);
  emon1.current(ADC_INPUT, 111.1); // ADC channel, default parameters
  pinMode(LED1, OUTPUT);
  pinMode(SSR, OUTPUT);
  digitalWrite(SSR, 0);
  digitalWrite(LED1, 1);
  pinMode(soilMoisturePin, INPUT);
  LoRa_init();
  dataSensor = Read_data();
  LoRa.onReceive(onReceive);
  LoRa.receive(); // Sẵn sàng nhận dữ liệu
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    dataSensor = Read_data();
    Serial.println(dataSensor); // In dữ liệu đọc được từ cảm biến ra Serial Monitor
  }
  if (thuchien)
  {
    if (on_off == Dv_ON)
    {
      ON_SSR();
      double Irms = emon1.calcIrms(1480); // Calculate Irms only
      if (Irms > 2)
      {
        for (int i = 0; i < 5; i++)
        {
          LoRa_sendMessage("@");
          delay(100);
        }
      }
    }
    else if (on_off == Dv_OFF)
    {
      OFF_SSR();
      double Irms = emon1.calcIrms(1480); // Calculate Irms only
      if (Irms < 2)
      {
        for (int i = 0; i < 5; i++)
        {
          LoRa_sendMessage("#"); 
          delay(100);
        }
      }
    }
    thuchien = 0;
    LoRa.receive(); // Sẵn sàng nhận dữ liệu
  }
}
