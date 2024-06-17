#pragma once

#include "gpio.h" // Assuming this header file contains the necessary pin definitions
#include <Arduino.h> // Include Arduino core library for analogRead() and Serial
#include "SHT3x.h"

SHT3x Sensor;
String temp;
String humi;

int averageAnalogRead(int pinToRead)
{
    byte numberOfReadings = 8;
    unsigned int runningValue = 0;
    for (int x = 0; x < numberOfReadings; x++)
        runningValue += analogRead(pinToRead);
    runningValue /= numberOfReadings;
    return (runningValue);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

String read_8051()
{
    // Define UVOUT and REF_3V3 if not defined already in "gpio.h"
    int uvLevel = averageAnalogRead(UVOUT);
    int refLevel = averageAnalogRead(REF_3V3);

    // Use the 3.3V power pin as a reference to get a very accurate output value from sensor
    float outputVoltage = 3.3 / refLevel * uvLevel;
    float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); // Convert the voltage to a UV intensity level
    return String(uvIntensity);
}

String Read_data()
{
  Sensor.UpdateData();
  if ((Sensor.GetTemperature() != 0.00) && (Sensor.GetRelHumidity() != 0.00))
  {
    String temp = String(Sensor.GetTemperature());
    String humi = String(Sensor.GetAbsHumidity());

    String DATA_SENSOR = temp + "@" + humi + "*" + read_8051();
    return DATA_SENSOR;
  }
  else
  {
    return String("ERROR SHT30"); // Error message if SHT3x sensor data unavailable
  }
}