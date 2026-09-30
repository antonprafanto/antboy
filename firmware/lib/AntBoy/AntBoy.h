#pragma once
#include <Arduino.h>
#include "AntBoy_Pins.h"
#include "AntBoy_Buttons.h"
#include "AntBoy_Display.h"
#include "AntBoy_Audio.h"
#include "AntBoy_SD.h"
#include "AntBoy_Power.h"

class AntBoyClass {
public:
    AntBoy_DisplayClass Display;
    AntBoy_ButtonsClass Buttons;
    AntBoy_AudioClass   Audio;
    AntBoy_SDClass      SD;
    AntBoy_PowerClass   Power;

    // Inisialisasi seluruh sistem ANTBOY
    void begin(bool initSD = true);

    // Wajib dipanggil di awal setiap iterasi loop()
    void update();

    // Helper Status LED D1 (IO2)
    void setLED(bool state);
    void toggleLED();

    // Deteksi kombinasi global tahan SELECT + START selama 2 detik (Exit/Reset)
    bool checkExitShortcut();

private:
    bool _ledState = false;
};

extern AntBoyClass AntBoy;
