#ifndef MAIN_H_
#define MAIN_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "WiFiManager.h"
#include "FIREBASE_MT.h"
#include "SPI.h"
#include "LoRa.h"
#include "OneButton.h"
#include "gpio_mt.h"
#include "feedback_led.h"
#include "WIFI_MT.h"
#include "realtime.h"
#include "lora_functions.h"
#include "button_custom.h"
bool datacurrent;
unsigned long GetModepreviousMillis;
void init_setup()
{
    Serial.begin(115200);
    init_gpio();
    init_wifi();
    FireBase_Init();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    init_date();
    init_time();
    lora_init();
    LoRa.receive();
    init_button();
}

#endif