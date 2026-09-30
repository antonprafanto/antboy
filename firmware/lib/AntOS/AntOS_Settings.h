#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include <AntBoy.h>

class AntOS_SettingsClass {
public:
    uint8_t  brightness = 80;    // 20, 40, 60, 80, 100%
    uint8_t  volume = 3;        // 0=MUTE, 1=LOW, 2=MED, 3=HIGH, 4=MAX
    bool     soundEnabled = true;
    uint16_t autoDimSeconds = 30;

    void begin();
    void load();
    void save();
    void apply();

    // Helper penyesuaian cepat
    uint8_t cycleBrightness();
    uint8_t cycleVolume();

private:
    Preferences _prefs;
};

extern AntOS_SettingsClass AntOS_Settings;
