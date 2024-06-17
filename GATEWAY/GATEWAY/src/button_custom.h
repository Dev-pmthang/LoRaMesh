#ifndef BUTTON_CUSTOM_H
#define BUTTON_CUSTOM_H

#include "main.h"

OneButton button0(BT_WF, true, true);
OneButton button1(BT1, true, true);
OneButton button2(BT2, true, true);
OneButton button3(BT3, true, true);

void init_button()
{
    button0.attachClick(sw_mode);
    button0.attachLongPressStart(clear_wifi);
    // button1.attachLongPressStart(sendnode1);
    // button2.attachLongPressStart(sendoff1);
    button3.attachLongPressStart(sendoff1);

    // button1.attachClick(sendon1);
    // button2.attachClick(sendon1);
    button3.attachClick(sendon1);

    // button1.attachDoubleClick(sendoff1);
    // button2.attachDoubleClick(sendoff1);
    // button3.attachDoubleClick(sendoff1);
}

void usebutton()
{
    button1.tick();
    button2.tick();
    button3.tick();
}

#endif