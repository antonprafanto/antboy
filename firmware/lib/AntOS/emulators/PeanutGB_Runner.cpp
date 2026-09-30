#include "PeanutGB_Runner.h"

PeanutGB_RunnerClass PeanutGB_Runner;

// 4 Retro Palettes for Game Boy Classic DMG (RGB565)
static const uint16_t GB_PALETTES[4][4] = {
    // 0: Classic DMG Olive Green
    { 0x9EE7, 0x86A5, 0x34A3, 0x09E1 },
    // 1: Pocket B&W
    { 0xFFFF, 0xAD55, 0x52AA, 0x0000 },
    // 2: Cyberpunk Neon
    { 0x07FF, 0x03E0, 0xF81F, 0x0000 },
    // 3: Amber Phosphor
    { 0xFDC0, 0xD400, 0x8A00, 0x1800 }
};

static const char* PALETTE_NAMES[4] = {
    "DMG Olive", "Pocket B&W", "Cyber Neon", "Amber"
};

// Built-in Demo ROM (Minimal Game Boy Header & Visual Test Loop)
static const uint8_t BUILTIN_DEMO_ROM[256] = {
    0x00, 0xC3, 0x50, 0x01, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83,
    0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6,
    0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F,
    0xBB, 0xB9, 0x33, 0x3E, 0x41, 0x4E, 0x54, 0x42, 0x4F, 0x59, 0x20, 0x47, 0x42, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t PeanutGB_RunnerClass::romReadCallback(struct gb_s *gb, const uint32_t addr) {
    PeanutGB_RunnerClass* runner = (PeanutGB_RunnerClass*)gb->direct;
    if (!runner) return 0xFF;

    if (!runner->hasRomLoaded) {
        if (addr < sizeof(BUILTIN_DEMO_ROM)) {
            return BUILTIN_DEMO_ROM[addr];
        }
        return 0x00;
    }

    if (addr < 0x4000) {
        if (runner->romBank0) return runner->romBank0[addr];
    } else {
        uint8_t bank = gb->cart_rom_bank;
        if (bank == 0) bank = 1;
        if (runner->curBankN != bank && runner->romBankN && runner->romFile) {
            runner->curBankN = bank;
            if (AntBoy.SD.lockBus(100)) {
                runner->romFile.seek((uint32_t)bank * 0x4000);
                runner->romFile.read(runner->romBankN, 0x4000);
                AntBoy.SD.unlockBus();
            }
        }
        if (runner->romBankN) {
            return runner->romBankN[addr - 0x4000];
        }
    }

    return 0xFF;
}

uint8_t PeanutGB_RunnerClass::ramReadCallback(struct gb_s *gb, const uint32_t addr) {
    return 0xFF;
}

void PeanutGB_RunnerClass::ramWriteCallback(struct gb_s *gb, const uint32_t addr, const uint8_t val) {
}

void PeanutGB_RunnerClass::errorCallback(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t addr) {
}

void PeanutGB_RunnerClass::drawLineCallback(struct gb_s *gb, const uint8_t pixels[160], const uint_fast8_t line) {
    PeanutGB_RunnerClass* runner = (PeanutGB_RunnerClass*)gb->direct;
    if (!runner) return;

    // Buffer garis RGB565 (160 piksel Game Boy)
    uint16_t lineBuf[160];
    const uint16_t* pal = GB_PALETTES[runner->currentPalette];

    for (int i = 0; i < 160; i++) {
        lineBuf[i] = pal[pixels[i] & 3];
    }

    // Gambar di tengah layar ST7789 (320x240): X=80, Y=48 + line
    AntBoy.Display.drawRGBBitmap(80, 48 + line, lineBuf, 160, 1);
}

void PeanutGB_RunnerClass::scanRoms() {
    romCount = 0;
    selectedRomIndex = 0;

    if (!AntBoy.SD.isMounted()) {
        AntBoy.SD.begin();
    }

    if (!AntBoy.SD.isMounted()) return;

    if (!AntBoy.SD.lockBus(200)) return;

    File dir = SD.open("/roms/gb");
    if (dir && dir.isDirectory()) {
        File file = dir.openNextFile();
        while (file && romCount < 16) {
            if (!file.isDirectory()) {
                const char* name = file.name();
                int len = strlen(name);
                if (len > 3 && (strcasecmp(name + len - 3, ".gb") == 0 || strcasecmp(name + len - 4, ".gbc") == 0)) {
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

bool PeanutGB_RunnerClass::loadRom(const char* filename) {
    closeRom();

    if (!AntBoy.SD.lockBus(200)) return false;

    char fullPath[64];
    snprintf(fullPath, sizeof(fullPath), "/roms/gb/%s", filename);
    romFile = SD.open(fullPath, FILE_READ);

    if (!romFile) {
        AntBoy.SD.unlockBus();
        return false;
    }

    romBank0 = (uint8_t*)malloc(0x4000); // 16KB Bank 0
    romBankN = (uint8_t*)malloc(0x4000); // 16KB Bank N

    if (!romBank0 || !romBankN) {
        closeRom();
        AntBoy.SD.unlockBus();
        return false;
    }

    romFile.seek(0);
    romFile.read(romBank0, 0x4000);
    curBankN = 1;
    romFile.seek(0x4000);
    romFile.read(romBankN, 0x4000);

    AntBoy.SD.unlockBus();
    hasRomLoaded = true;

    gb_init(&gb, romReadCallback, ramReadCallback, ramWriteCallback, errorCallback, this);
    gb_init_lcd(&gb, drawLineCallback);

    return true;
}

void PeanutGB_RunnerClass::closeRom() {
    if (hasRomLoaded) {
        if (romFile) {
            if (AntBoy.SD.lockBus(100)) {
                romFile.close();
                AntBoy.SD.unlockBus();
            }
        }
        if (romBank0) { free(romBank0); romBank0 = nullptr; }
        if (romBankN) { free(romBankN); romBankN = nullptr; }
        hasRomLoaded = false;
    }
}

void PeanutGB_RunnerClass::renderBrowser() {
    // Header
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 26, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 26, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_PIL_GAMING);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(10, 9);
    AntBoy.Display.print("< [B] BACK");

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("GAME BOY (PEANUT-GB)", 9, ANTOS_COLOR_WHITE, 1);

    // Sub-header info
    AntBoy.Display.fillRect(0, 27, ANTBOY_SCREEN_WIDTH, 18, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(AntBoy.SD.isMounted() ? ANTOS_COLOR_GREEN : ANTOS_COLOR_RED, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.setCursor(12, 32);
    AntBoy.Display.printf("SD: %s  |  FOLDER: /roms/gb/  |  ROMS: %d", AntBoy.SD.isMounted() ? "READY" : "NO SD", romCount);

    if (romCount == 0) {
        // Kartu panduan memasukkan ROM
        int guideY = 56;
        AntBoy.Display.fillRoundRect(16, guideY, 288, 140, 6, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawRoundRect(16, guideY, 288, 140, 6, ANTOS_COLOR_PIL_GAMING);

        AntBoy.Display.setTextSize(1);
        AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawCenteredText("PANDUAN MEMASANG GAME:", guideY + 12, ANTOS_COLOR_WHITE, 1);

        AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.setCursor(26, guideY + 34);
        AntBoy.Display.print("1. Lepas MicroSD dan pasang di PC.");

        AntBoy.Display.setCursor(26, guideY + 50);
        AntBoy.Display.print("2. Buka folder /roms/gb/ di MicroSD.");

        AntBoy.Display.setCursor(26, guideY + 66);
        AntBoy.Display.print("3. Salin file game .gb atau .gbc.");

        AntBoy.Display.setCursor(26, guideY + 82);
        AntBoy.Display.print("4. Pasang kembali MicroSD ke ANTBOY.");

        // Tombol jalankan demo
        AntBoy.Display.fillRoundRect(36, guideY + 104, 248, 24, 4, ANTOS_COLOR_BG_CARD_ACTIVE);
        AntBoy.Display.drawRoundRect(36, guideY + 104, 248, 24, 4, ANTOS_COLOR_GREEN);
        AntBoy.Display.setTextColor(ANTOS_COLOR_GREEN, ANTOS_COLOR_BG_CARD_ACTIVE);
        AntBoy.Display.drawCenteredText("[A] Jalankan Built-in Core Test", guideY + 112, ANTOS_COLOR_GREEN, 1);
    } else {
        // List ROM yang ditemukan di kartu SD
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

            // Cursor
            if (selected) {
                AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, bg);
                AntBoy.Display.setCursor(24, curY + 9);
                AntBoy.Display.print(">");
            }

            // Nama ROM
            AntBoy.Display.setTextColor(selected ? ANTOS_COLOR_WHITE : ANTOS_COLOR_TEXT_DIM, bg);
            AntBoy.Display.setCursor(38, curY + 9);
            AntBoy.Display.print(romFileList[i]);

            // Ukuran file (KB)
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
    AntBoy.Display.drawCenteredText("D-Pad [^/v] Pilih | [A] Mulai Game | [B] Kembali", fy + 6, ANTOS_COLOR_TEXT_DIM, 1);
}

void PeanutGB_RunnerClass::runEmulationLoop() {
    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);

    // Gambar bezel Game Boy Cyberpunk di sekeliling area 160x144 (tengah)
    int frameX = 76;
    int frameY = 44;
    int frameW = 168;
    int frameH = 152;
    AntBoy.Display.fillRoundRect(frameX, frameY, frameW, frameH, 6, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(frameX, frameY, frameW, frameH, 6, ANTOS_COLOR_PIL_GAMING);
    AntBoy.Display.fillRect(80, 48, 160, 144, 0x0000); // Screen area black

    // Header info bar
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 20, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 20, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_PIL_GAMING);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(10, 6);
    AntBoy.Display.print("GAME BOY 60 FPS");

    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(180, 6);
    AntBoy.Display.printf("PAL: %s [MENU]", PALETTE_NAMES[currentPalette]);

    // Footer info
    int fy = ANTBOY_SCREEN_HEIGHT - 18;
    AntBoy.Display.fillRect(0, fy, ANTBOY_SCREEN_WIDTH, 18, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("SELECT + START: Keluar ke Launcher", fy + 5, ANTOS_COLOR_TEXT_DIM, 1);

    bool running = true;
    uint32_t lastFrame = millis();

    while (running) {
        AntBoy.update();

        // Global Exit Shortcut
        if (AntBoy.checkExitShortcut()) {
            break;
        }

        // Ganti Palette dengan Tombol MENU
        if (AntBoy.Buttons.wasPressed(ANT_BTN_MENU)) {
            currentPalette = (currentPalette + 1) % 4;
            AntBoy.Audio.playClick();
            AntBoy.Display.fillRect(180, 6, 130, 12, ANTOS_COLOR_BG_PANEL);
            AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
            AntBoy.Display.setCursor(180, 6);
            AntBoy.Display.printf("PAL: %s [MENU]", PALETTE_NAMES[currentPalette]);
        }

        // Map Kontrol Tombol Fisik ANTBOY ke Joypad Game Boy
        gb.joypad.bits.a      = AntBoy.Buttons.isPressed(ANT_BTN_A);
        gb.joypad.bits.b      = AntBoy.Buttons.isPressed(ANT_BTN_B);
        gb.joypad.bits.select = AntBoy.Buttons.isPressed(ANT_BTN_SELECT);
        gb.joypad.bits.start  = AntBoy.Buttons.isPressed(ANT_BTN_START);
        gb.joypad.bits.up     = AntBoy.Buttons.isPressed(ANT_BTN_UP);
        gb.joypad.bits.down   = AntBoy.Buttons.isPressed(ANT_BTN_DOWN);
        gb.joypad.bits.left   = AntBoy.Buttons.isPressed(ANT_BTN_LEFT);
        gb.joypad.bits.right  = AntBoy.Buttons.isPressed(ANT_BTN_RIGHT);

        // Jalankan 1 frame emulasi
        gb_run_frame(&gb);

        // Target 60 FPS pacing (~16.6ms per frame)
        uint32_t elapsed = millis() - lastFrame;
        if (elapsed < 16) {
            delay(16 - elapsed);
        }
        lastFrame = millis();
    }

    closeRom();
}

void PeanutGB_RunnerClass::run() {
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
                if (loadRom(romFileList[selectedRomIndex])) {
                    runEmulationLoop();
                }
            } else {
                // Jalankan Built-in Core Test
                hasRomLoaded = false;
                gb_init(&gb, romReadCallback, ramReadCallback, ramWriteCallback, errorCallback, this);
                gb_init_lcd(&gb, drawLineCallback);
                runEmulationLoop();
            }
            AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
            renderBrowser();
        } else if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
            AntBoy.Audio.playClick();
            inBrowser = false;
        }

        delay(15);
    }
}
