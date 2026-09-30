#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "AntOS_Theme.h"

class AntOS_StatusBarClass {
public:
    void begin();
    void update();
    void render(bool forceRedraw = false);

    // OSD Volume Toast popup
    void triggerVolumeOSD(const char* volStr, uint8_t level);
    bool isToastActive() const { return _toastActive; }
    bool checkToastClosed();
    void renderToast();

private:
    uint32_t _lastRenderTime = 0;
    uint32_t _toastDismissTime = 0;
    bool     _toastActive = false;
    bool     _toastJustClosed = false;
    char     _toastVolStr[16] = {0};
    uint8_t  _toastVolLevel = 0;

    bool     _lastSDState = false;
    uint8_t  _lastVol = 255;
    uint16_t _lastSecs = 0xFFFF;

    void drawSDIcon(int x, int y, bool mounted);
    void drawWifiIcon(int x, int y, bool active);
    void drawVolumeIcon(int x, int y, uint8_t level);
    void drawBatteryIcon(int x, int y);
};

extern AntOS_StatusBarClass AntOS_StatusBar;
