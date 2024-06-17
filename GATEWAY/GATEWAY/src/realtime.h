#ifndef REALTIME_H
#define REALTIME_H

#include "main.h"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // GMT+7
const int daylightOffset_sec = 0;    // Không có ánh sáng ban ngày tiết kiệm
struct tm timeinfo;

String init_time()
{
    time_t datetime;
    if (!getLocalTime(&timeinfo))
    {
        Serial.print("Time fail");
    }

    time(&datetime);
    char timertc[32];
    strftime(timertc, sizeof timertc, "%H:%M:%S", &timeinfo);
    return String(timertc);
}

String init_date()
{
    time_t datetime;
    if (!getLocalTime(&timeinfo))
    {
        Serial.print("Date fail");
    }
    time(&datetime);
    char date[32];
    strftime(date, sizeof date, "%d-%m-%Y", &timeinfo);
    return date;
}

#endif