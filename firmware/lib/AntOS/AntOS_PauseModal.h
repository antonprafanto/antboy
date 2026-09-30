#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "AntOS_Theme.h"

inline bool AntOS_ShowPauseMenu(const char* gameTitle) {
    AntBoy.Audio.playClick();

    int modalW = 230;
    int modalH = 96;
    int mx = (ANTBOY_SCREEN_WIDTH - modalW) / 2;
    int my = (ANTBOY_SCREEN_HEIGHT - modalH) / 2;

    AntBoy.Display.fillRoundRect(mx, my, modalW, modalH, 6, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(mx, my, modalW, modalH, 6, ANTOS_COLOR_PIL_GAMING);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("== PAUSED ==", my + 14, ANTOS_COLOR_YELLOW, 1);

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText(gameTitle, my + 30, ANTOS_COLOR_WHITE, 1);

    AntBoy.Display.setTextColor(ANTOS_COLOR_GREEN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("[A] / [MENU] Lanjutkan", my + 52, ANTOS_COLOR_GREEN, 1);

    AntBoy.Display.setTextColor(ANTOS_COLOR_RED, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("[B] Keluar ke Launcher", my + 72, ANTOS_COLOR_RED, 1);

    delay(250); // Debounce rilis tombol MENU

    while (true) {
        AntBoy.update();

        // Lanjut main jika tombol A atau MENU ditekan
        if (AntBoy.Buttons.wasPressed(ANT_BTN_A) || AntBoy.Buttons.wasPressed(ANT_BTN_MENU)) {
            AntBoy.Audio.playConfirm();
            return true; // Resume
        }

        // Keluar jika tombol B atau shortcut ditekan
        if (AntBoy.Buttons.wasPressed(ANT_BTN_B) || AntBoy.checkExitShortcut()) {
            AntBoy.Audio.playClick();
            return false; // Exit to launcher
        }

        delay(15);
    }
}
