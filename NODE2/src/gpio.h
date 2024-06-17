#pragma once

#include <Arduino.h>

int UVOUT = 15;  // Output from the sensor
int REF_3V3 = 4; // 3.3V power on the ESP32 board

void init_gpio()
{
    pinMode(UVOUT, INPUT);
    pinMode(REF_3V3, OUTPUT);
    digitalWrite(REF_3V3,HIGH);
}

