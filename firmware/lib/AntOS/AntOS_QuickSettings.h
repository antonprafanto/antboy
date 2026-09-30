#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "AntOS_Theme.h"
#include "AntOS_Settings.h"

class AntOS_QuickSettingsClass {
public:
    void begin();
    bool isOpen() const { return _isOpen; }
    void open();
    void close();
    void toggle();

    // Mengembalikan true jika event input tombol di-handle oleh modal
    bool handleInput();
    void render();

private:
    bool    _isOpen = false;
    uint8_t _selectedIndex = 0; // 0=Brightness, 1=Volume, 2=Sleep, 3=Info, 4=Close
    uint8_t _menuItemCount = 5;

    void drawItem(int x, int y, int w, int h, int index, const char* label, const char* value, bool selected);
};

extern AntOS_QuickSettingsClass AntOS_QuickSettings;
