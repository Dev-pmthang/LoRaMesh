#ifndef WIFI_MT_H
#define WIFI_MT_H

#include "main.h"

WiFiManager wifiManager;

const char *ssid = "EPS32_MT";
const char *password = "12345678";

unsigned long time_LED_Connect_WiFi = millis();    // lay gia tri tu timer
unsigned long timeInterval_LED_Connect_WiFi = 500; // thoi gian can su dung 500 = 0.5s
// ket noi lai wifi
unsigned long time_Reconnect_WiFi = millis();
unsigned long timeInterval_Reconnect_WiFi = 10000; // thoi gian 10s ket noi lai 1 lan


void blink_led()
{
  if (WiFi.status() == WL_CONNECTED && millis() - time_LED_Connect_WiFi > timeInterval_LED_Connect_WiFi)
  {
    digitalWrite(LED_WF, !digitalRead(LED_WF)); // dao trang thai led se nhap nhay sau khi ket noi
    time_LED_Connect_WiFi = millis();
  } // dao trang thai led ko dc sd delay
}

void clear_wifi()
{
  blink_led();
  wifiManager.resetSettings();
  ESP.restart();
}

void init_wifi()
{
  digitalWrite(LED_WF, HIGH);           // cho led sáng lên
  WiFi.mode(WIFI_STA);                  // chọn mode wifi
  while (WiFi.status() != WL_CONNECTED) // nếu không kết nối được đến wifi thì nó đợi 2 phút
  {
    wifiManager.setConfigPortalTimeout(120); // hàm sét đợi 2 phút
    wifiManager.autoConnect(ssid, password); // wifi do esp32 phát ra để cấu hình
  }
}

void reconnect_WiFi()
{
  if (WiFi.status() != WL_CONNECTED) // neu mat ket noi WIFI
  {
    if (millis() - time_Reconnect_WiFi > timeInterval_Reconnect_WiFi) // timer sau 10s thi ket noi lai 1 lan
    {
      digitalWrite(LED_WF, HIGH);                // neu mat ket noi led sang lun
      WiFi.disconnect();                         // ngat ket noi wifi
      WiFi.reconnect();                          // thuc hien ket noi lai
      Serial.println("Reconnecting to WiFi..."); // log ra man hinh
      time_Reconnect_WiFi = millis();            // gan lai gia tri timer
    }
  }
}

#endif