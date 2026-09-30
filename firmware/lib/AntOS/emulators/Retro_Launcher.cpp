#include "Retro_Launcher.h"

Retro_LauncherClass Retro_Launcher;

static const char* CONSOLE_NAMES[3] = {
    "NES / FAMICOM (8-BIT)",
    "SEGA MASTER SYSTEM & GG",
    "ATARI 2600 & CHIP-8 VM"
};

static const char* CONSOLE_PATHS[3] = {
    "/roms/nes",
    "/roms/sms",
    "/roms/atari"
};

static const char* CONSOLE_EXTS[3] = {
    ".nes",
    ".sms",
    ".ch8"
};

void Retro_LauncherClass::scanRoms() {
    romCount = 0;
    selectedRomIndex = 0;

    if (!AntBoy.SD.isMounted()) {
        AntBoy.SD.begin();
    }
    if (!AntBoy.SD.isMounted()) return;

    if (!AntBoy.SD.lockBus(200)) return;

    File dir = SD.open(CONSOLE_PATHS[currentConsole]);
    if (dir && dir.isDirectory()) {
        File file = dir.openNextFile();
        while (file && romCount < 16) {
            if (!file.isDirectory()) {
                const char* name = file.name();
                int len = strlen(name);
                const char* ext = CONSOLE_EXTS[currentConsole];
                int extLen = strlen(ext);
                if (len > extLen && strcasecmp(name + len - extLen, ext) == 0) {
                    strncpy(romFileList[romCount], name, sizeof(romFileList[romCount]) - 1);
                    romFileSizes[romCount] = file.size();
                    romCount++;
                }
            }
            file = dir.openNextFile();
        }
        dir.close();
    }

    AntBoy.SD.unlockBus();
}

void Retro_LauncherClass::renderBrowser() {
    // Header
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 26, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 26, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_PIL_GAMING);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(10, 9);
    AntBoy.Display.print("< [B] BACK");

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText(CONSOLE_NAMES[currentConsole], 9, ANTOS_COLOR_WHITE, 1);

    // Status bar
    AntBoy.Display.fillRect(0, 27, ANTBOY_SCREEN_WIDTH, 18, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(AntBoy.SD.isMounted() ? ANTOS_COLOR_GREEN : ANTOS_COLOR_RED, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.setCursor(12, 32);
    AntBoy.Display.printf("SD: %s  |  FOLDER: %s/  |  ROMS: %d", AntBoy.SD.isMounted() ? "READY" : "NO SD", CONSOLE_PATHS[currentConsole], romCount);

    if (romCount == 0) {
        int guideY = 56;
        AntBoy.Display.fillRoundRect(16, guideY, 288, 140, 6, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawRoundRect(16, guideY, 288, 140, 6, ANTOS_COLOR_PIL_GAMING);

        AntBoy.Display.setTextSize(1);
        AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawCenteredText("PANDUAN MEMASANG ROM:", guideY + 12, ANTOS_COLOR_WHITE, 1);

        char guideBuf[64];
        snprintf(guideBuf, sizeof(guideBuf), "1. Buka folder %s/ di MicroSD.", CONSOLE_PATHS[currentConsole]);
        AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.setCursor(24, guideY + 34);
        AntBoy.Display.print(guideBuf);

        snprintf(guideBuf, sizeof(guideBuf), "2. Salin file ROM (%s).", CONSOLE_EXTS[currentConsole]);
        AntBoy.Display.setCursor(24, guideY + 50);
        AntBoy.Display.print(guideBuf);

        AntBoy.Display.setCursor(24, guideY + 66);
        AntBoy.Display.print("3. Pasang MicroSD kembali ke ANTBOY.");

        AntBoy.Display.setCursor(24, guideY + 82);
        AntBoy.Display.print("4. Tekan [A] untuk memindai ulang.");

        // Jika CHIP-8/Atari, tawarkan built-in Pong runner
        if (currentConsole == RETRO_TYPE_ATARI) {
            AntBoy.Display.fillRoundRect(36, guideY + 104, 248, 24, 4, ANTOS_COLOR_BG_CARD_ACTIVE);
            AntBoy.Display.drawRoundRect(36, guideY + 104, 248, 24, 4, ANTOS_COLOR_GREEN);
            AntBoy.Display.setTextColor(ANTOS_COLOR_GREEN, ANTOS_COLOR_BG_CARD_ACTIVE);
            AntBoy.Display.drawCenteredText("[A] Jalankan Built-in Retro VM", guideY + 112, ANTOS_COLOR_GREEN, 1);
        }
    } else {
        int startY = 50;
        int itemH = 26;
        int spacing = 28;

        for (int i = 0; i < romCount && i < 5; i++) {
            bool selected = (selectedRomIndex == i);
            int curY = startY + (i * spacing);
            uint16_t bg = selected ? ANTOS_COLOR_BG_CARD_ACTIVE : ANTOS_COLOR_BG_PANEL;
            uint16_t border = selected ? ANTOS_COLOR_PIL_GAMING : ANTOS_COLOR_BORDER_DIM;

            AntBoy.Display.fillRoundRect(16, curY, 288, itemH, 4, bg);
            AntBoy.Display.drawRoundRect(16, curY, 288, itemH, 4, border);

            if (selected) {
                AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, bg);
                AntBoy.Display.setCursor(24, curY + 9);
                AntBoy.Display.print(">");
            }

            AntBoy.Display.setTextColor(selected ? ANTOS_COLOR_WHITE : ANTOS_COLOR_TEXT_DIM, bg);
            AntBoy.Display.setCursor(38, curY + 9);
            AntBoy.Display.print(romFileList[i]);

            AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, bg);
            AntBoy.Display.setCursor(240, curY + 9);
            AntBoy.Display.printf("%d KB", romFileSizes[i] / 1024);
        }
    }

    // Footer
    int fy = ANTBOY_SCREEN_HEIGHT - 20;
    AntBoy.Display.fillRect(0, fy, ANTBOY_SCREEN_WIDTH, 20, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, fy, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_BORDER_DIM);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("D-Pad [^/v] Pilih | [A] Pilih ROM | [B] Kembali", fy + 6, ANTOS_COLOR_TEXT_DIM, 1);
}

void Retro_LauncherClass::showRomInfo(const char* filename) {
    if (currentConsole == RETRO_TYPE_ATARI) {
        char fullPath[64];
        snprintf(fullPath, sizeof(fullPath), "/roms/atari/%s", filename);
        Chip8_Runner.run(fullPath);
        return;
    }

    // Untuk NES dan SMS: Tampilkan status engine yang transparan dan bersahabat
    AntBoy.Display.fillRoundRect(20, 45, 280, 150, 6, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(20, 45, 280, 150, 6, ANTOS_COLOR_YELLOW);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("== STATUS ENGINE NES / SMS ==", 57, ANTOS_COLOR_YELLOW, 1);

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    char fBuf[64];
    snprintf(fBuf, sizeof(fBuf), "FILE: %s", filename);
    AntBoy.Display.drawCenteredText(fBuf, 75, ANTOS_COLOR_WHITE, 1);

    AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("Hardware ESP32 Wemos D1 Mini32", 95, ANTOS_COLOR_CYAN, 1);
    AntBoy.Display.drawCenteredText("memiliki SRAM 320 KB (tanpa PSRAM).", 109, ANTOS_COLOR_CYAN, 1);
    AntBoy.Display.drawCenteredText("Core NES Nofrendo butuh mapper eksternal.", 123, ANTOS_COLOR_CYAN, 1);

    AntBoy.Display.setTextColor(ANTOS_COLOR_GREEN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("Mainkan game favoritmu Mario & Retro", 143, ANTOS_COLOR_GREEN, 1);
    AntBoy.Display.drawCenteredText("di menu GAME BOY (/roms/gb/) 60 FPS!", 157, ANTOS_COLOR_GREEN, 1);

    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("Tekan tombol [B] untuk kembali", 177, ANTOS_COLOR_TEXT_DIM, 1);

    while (true) {
        AntBoy.update();
        if (AntBoy.Buttons.wasPressed(ANT_BTN_B) || AntBoy.Buttons.wasPressed(ANT_BTN_A) || AntBoy.Buttons.wasPressed(ANT_BTN_MENU)) {
            break;
        }
        delay(20);
    }
}

void Retro_LauncherClass::run(RetroConsoleType consoleType) {
    currentConsole = consoleType;
    scanRoms();
    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
    renderBrowser();

    bool inBrowser = true;

    while (inBrowser) {
        AntBoy.update();

        if (AntBoy.checkExitShortcut()) {
            break;
        }

        if (romCount > 0) {
            if (AntBoy.Buttons.wasPressed(ANT_BTN_UP)) {
                selectedRomIndex = (selectedRomIndex > 0) ? selectedRomIndex - 1 : romCount - 1;
                AntBoy.Audio.playTone(2637, 15);
                renderBrowser();
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_DOWN)) {
                selectedRomIndex = (selectedRomIndex + 1) % romCount;
                AntBoy.Audio.playTone(2637, 15);
                renderBrowser();
            }
        }

        if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
            AntBoy.Audio.playConfirm();
            if (romCount > 0) {
                showRomInfo(romFileList[selectedRomIndex]);
            } else if (currentConsole == RETRO_TYPE_ATARI) {
                Chip8_Runner.run();
            } else {
                scanRoms();
            }
            AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
            renderBrowser();
        } else if (AntBoy.Buttons.wasPressed(ANT_BTN_B) || AntBoy.Buttons.wasPressed(ANT_BTN_MENU)) {
            AntBoy.Audio.playClick();
            inBrowser = false;
        }

        delay(15);
    }
}
