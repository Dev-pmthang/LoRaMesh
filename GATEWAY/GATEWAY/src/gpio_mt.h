#ifndef GPIO_H
#define GPIO_H

#include "main.h"

#define BT_WF 0
#define BT1 39
#define BT2 34
#define BT3 35
#define SW 15

#define LED_WF 2
#define LED1 32
#define LED2 33
#define LED3 25

#define NSS 5
#define RST 14
#define DI0 27

void init_gpio()
{
    pinMode(BT_WF, INPUT_PULLUP);
    pinMode(BT1, INPUT);
    pinMode(BT2, INPUT);
    pinMode(BT3, INPUT);
    pinMode(SW,INPUT_PULLUP);

    pinMode(LED_WF, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    digitalWrite(LED1,1);
    digitalWrite(LED2,1);
    digitalWrite(LED3,1);
}

#endif