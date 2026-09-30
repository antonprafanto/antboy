#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "AntOS_Theme.h"

enum AntOSPillar {
    ANTOS_PIL_GAMING = 0,
    ANTOS_PIL_WIRELESS,
    ANTOS_PIL_IOT,
    ANTOS_PIL_LAB,
    ANTOS_PIL_COUNT
};

struct PillarData {
    const char* title;
    const char* subtitle;
    uint16_t    themeColor;
    const char* appList[4];
};

class AntOS_LauncherClass {
public:
    void begin();
    void update();
    void render(bool forceRedraw = false);

    bool handleInput();
    bool isInSubMenu() const { return _inSubMenu; }

private:
    uint8_t _currentPillar = 0;
    bool    _inSubMenu = false;
    uint8_t _subItemIndex = 0;
    bool    _needsRedraw = true;

    void drawCarousel(int slideX = 0);
    void drawCard(int x, int y, int w, int h, uint8_t pillarIndex, bool isFocused);
    void drawPillarIcon(int cx, int cy, uint8_t pillarIndex, uint16_t color);
    void drawSubMenu();
    void drawFooterGuide();
};

extern AntOS_LauncherClass AntOS_Launcher;
