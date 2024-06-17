#ifndef FEEDBACK_LED_H
#define FEEDBACK_LED_H

#include "main.h"

void fb_node1_on()
{
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(LED1, 0);
        delay(100);
        digitalWrite(LED1, 1);
        delay(100);
    }
}

void fb_node2()
{
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(LED2, 0);
        delay(100);
        digitalWrite(LED2, 1);
        delay(100);
    }
}

void fb_node3()
{
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(LED3, 0);
        delay(100);
        digitalWrite(LED3, 1);
        delay(100);
    }
}

#endif