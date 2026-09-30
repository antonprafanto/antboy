#pragma once
#include <Arduino.h>
#include "AntBoy_Pins.h"
#include "AntBoy_Buttons.h"

class AntBoy_PowerClass {
public:
    void begin();

    // Masuk ke mode Deep Sleep (Bangun via tombol MENU atau START)
    void sleep(AntButton wakeButton = ANT_BTN_MENU);

    // Cek penyebab wakeup
    bool wasWakeupFromButton() const;
};
