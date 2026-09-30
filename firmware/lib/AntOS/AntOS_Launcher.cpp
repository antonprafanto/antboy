#include "AntOS_Launcher.h"

AntOS_LauncherClass AntOS_Launcher;

static const PillarData s_pillars[ANTOS_PIL_COUNT] = {
    {
        "RETRO GAMING",
        "Peanut-GB & 8-Bit Arcade",
        ANTOS_COLOR_PIL_GAMING,
        { "Peanut-GB Emulator", "Snake Retro", "Tetris Pocket", "Chiptune Player" }
    },
    {
        "WIRELESS & CYBER",
        "Network Audit & BLE Hunter",
        ANTOS_COLOR_PIL_WIRELESS,
        { "Wi-Fi Spectrum Scan", "Packet Sniffer", "BLE Beacon Hunter", "BLE Gamepad HID" }
    },
    {
        "IoT SMART POCKET",
        "ESP-NOW Chat & Smart Home",
        ANTOS_COLOR_PIL_IOT,
        { "ESP-NOW Peer Chat", "MQTT Dashboard", "Pocket Clock & Weather", "Smart Relay Hub" }
    },
    {
        "HARDWARE LAB",
        "15-Pin J4 Expansion Tools",
        ANTOS_COLOR_PIL_LAB,
        { "UART Serial Monitor", "I2C Bus Scanner", "GPIO Logic Probe", "DAC Signal Generator" }
    }
};

void AntOS_LauncherClass::begin() {
    _currentPillar = 0;
    _inSubMenu = false;
    _subItemIndex = 0;
    _needsRedraw = true;
}

void AntOS_LauncherClass::drawPillarIcon(int cx, int cy, uint8_t pillarIndex, uint16_t color) {
    switch (pillarIndex) {
        case ANTOS_PIL_GAMING:
            // Ikon Konsol Game Boy Klasik
            AntBoy.Display.fillRoundRect(cx - 18, cy - 20, 36, 40, 4, color);
            AntBoy.Display.fillRoundRect(cx - 14, cy - 16, 28, 18, 2, ANTOS_COLOR_BG_PANEL); // Screen
            AntBoy.Display.fillRect(cx - 12, cy - 14, 24, 14, 0x9EE7);                       // GB LCD Olive
            // D-Pad
            AntBoy.Display.fillRect(cx - 11, cy + 6, 7, 3, ANTOS_COLOR_BG_PANEL);
            AntBoy.Display.fillRect(cx - 9, cy + 4, 3, 7, ANTOS_COLOR_BG_PANEL);
            // Buttons A/B
            AntBoy.Display.fillCircle(cx + 6, cy + 9, 2, ANTOS_COLOR_RED);
            AntBoy.Display.fillCircle(cx + 11, cy + 6, 2, ANTOS_COLOR_RED);
            break;

        case ANTOS_PIL_WIRELESS:
            // Ikon Gelombang Radio / Radar
            AntBoy.Display.fillCircle(cx, cy + 10, 3, color);
            // Arcs
            AntBoy.Display.drawCircle(cx, cy + 10, 8, color);
            AntBoy.Display.drawCircle(cx, cy + 10, 15, color);
            AntBoy.Display.drawCircle(cx, cy + 10, 22, color);
            // Tower mast
            AntBoy.Display.drawFastVLine(cx, cy + 10, 10, color);
            break;

        case ANTOS_PIL_IOT:
            // Ikon Jaringan Simpul IoT / Cloud Node
            AntBoy.Display.fillCircle(cx, cy, 6, color);
            // Satelit nodes
            AntBoy.Display.fillCircle(cx - 18, cy - 12, 4, ANTOS_COLOR_YELLOW);
            AntBoy.Display.fillCircle(cx + 18, cy - 12, 4, ANTOS_COLOR_YELLOW);
            AntBoy.Display.fillCircle(cx - 14, cy + 14, 4, ANTOS_COLOR_YELLOW);
            AntBoy.Display.fillCircle(cx + 14, cy + 14, 4, ANTOS_COLOR_YELLOW);
            // Connecting lines
            AntBoy.Display.drawLine(cx, cy, cx - 18, cy - 12, color);
            AntBoy.Display.drawLine(cx, cy, cx + 18, cy - 12, color);
            AntBoy.Display.drawLine(cx, cy, cx - 14, cy + 14, color);
            AntBoy.Display.drawLine(cx, cy, cx + 14, cy + 14, color);
            break;

        case ANTOS_PIL_LAB:
            // Ikon Header Pin & Gelombang Osiloskop
            AntBoy.Display.drawRect(cx - 20, cy - 16, 40, 32, color);
            // Pin indicators
            for (int p = 0; p < 4; p++) {
                AntBoy.Display.fillCircle(cx - 14 + (p * 9), cy - 11, 2, ANTOS_COLOR_YELLOW);
                AntBoy.Display.fillCircle(cx - 14 + (p * 9), cy + 11, 2, ANTOS_COLOR_YELLOW);
            }
            // Oscilloscope sine wave in center
            for (int wx = -12; wx < 12; wx++) {
                int wy = (int)(sin(wx * 0.4) * 5);
                AntBoy.Display.drawPixel(cx + wx, cy + wy, ANTOS_COLOR_GREEN);
            }
            break;
    }
}

void AntOS_LauncherClass::drawCard(int x, int y, int w, int h, uint8_t pillarIndex, bool isFocused) {
    const PillarData& data = s_pillars[pillarIndex];
    uint16_t bg = isFocused ? ANTOS_COLOR_BG_CARD_ACTIVE : ANTOS_COLOR_BG_PANEL;
    uint16_t border = isFocused ? data.themeColor : ANTOS_COLOR_BORDER_DIM;

    AntBoy.Display.fillRoundRect(x, y, w, h, 8, bg);
    AntBoy.Display.drawRoundRect(x, y, w, h, 8, border);
    if (isFocused) {
        AntBoy.Display.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 7, border);
    }

    if (!isFocused) return; // Kartu samping hanya menampilkan outline & shape dasar

    // Nomor Indikator Pilar [1/4]
    char idxBuf[12];
    snprintf(idxBuf, sizeof(idxBuf), "%d / %d", pillarIndex + 1, ANTOS_PIL_COUNT);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(data.themeColor, bg);
    AntBoy.Display.setCursor(x + w - 38, y + 8);
    AntBoy.Display.print(idxBuf);

    // Ikon Grafis Pilar
    drawPillarIcon(x + (w / 2), y + 42, pillarIndex, data.themeColor);

    // Judul Pilar
    AntBoy.Display.setTextSize(2);
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, bg);
    AntBoy.Display.drawCenteredText(data.title, y + 74, ANTOS_COLOR_WHITE, 1);

    // Garis Aksen Pembagi
    AntBoy.Display.drawFastHLine(x + 20, y + 88, w - 40, border);

    // Subjudul / Deskripsi Singkat
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, bg);
    AntBoy.Display.drawCenteredText(data.subtitle, y + 96, ANTOS_COLOR_TEXT_DIM, 1);

    // Badge "4 APPS READY"
    int pillW = 90;
    int pillX = x + ((w - pillW) / 2);
    int pillY = y + 114;
    AntBoy.Display.fillRoundRect(pillX, pillY, pillW, 16, 4, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(pillX, pillY, pillW, 16, 4, data.themeColor);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(data.themeColor, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("4 APPS READY", pillY + 4, data.themeColor, 1);
}

void AntOS_LauncherClass::drawCarousel() {
    // Bersihkan area konten (di bawah status bar, di atas footer)
    AntBoy.Display.fillRect(0, 23, ANTBOY_SCREEN_WIDTH, 193, ANTOS_COLOR_BG_DARK);

    // Kartu Kiri (Preview)
    uint8_t leftPillar = (_currentPillar > 0) ? _currentPillar - 1 : ANTOS_PIL_COUNT - 1;
    drawCard(-110, 42, 150, 134, leftPillar, false);

    // Kartu Kanan (Preview)
    uint8_t rightPillar = (_currentPillar + 1) % ANTOS_PIL_COUNT;
    drawCard(280, 42, 150, 134, rightPillar, false);

    // Kartu Utama Tengah (Fokus)
    drawCard(55, 34, 210, 148, _currentPillar, true);

    // Indikator Titik (Dots) di Bawah Kartu
    int startDotX = 136;
    int dotY = 196;
    for (int i = 0; i < ANTOS_PIL_COUNT; i++) {
        uint16_t dotCol = (i == _currentPillar) ? s_pillars[_currentPillar].themeColor : ANTOS_COLOR_BORDER_DIM;
        int dotR = (i == _currentPillar) ? 4 : 2;
        AntBoy.Display.fillCircle(startDotX + (i * 16), dotY, dotR, dotCol);
    }
}

void AntOS_LauncherClass::drawSubMenu() {
    const PillarData& data = s_pillars[_currentPillar];

    // Bersihkan area konten
    AntBoy.Display.fillRect(0, 23, ANTBOY_SCREEN_WIDTH, 193, ANTOS_COLOR_BG_DARK);

    // Sub-Header Pilar
    AntBoy.Display.fillRect(0, 24, ANTBOY_SCREEN_WIDTH, 22, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 46, ANTBOY_SCREEN_WIDTH, data.themeColor);

    // Tombol Back [B]
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(10, 31);
    AntBoy.Display.print("< [B] BACK");

    // Judul Pilar
    AntBoy.Display.setTextColor(data.themeColor, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText(data.title, 31, data.themeColor, 1);

    // 4 Item Aplikasi
    int startY = 56;
    int itemH = 30;
    int spacing = 35;
    int itemW = 280;
    int itemX = 20;

    for (int i = 0; i < 4; i++) {
        bool selected = (_subItemIndex == i);
        int curY = startY + (i * spacing);
        uint16_t bg = selected ? ANTOS_COLOR_BG_CARD_ACTIVE : ANTOS_COLOR_BG_PANEL;
        uint16_t border = selected ? data.themeColor : ANTOS_COLOR_BORDER_DIM;
        uint16_t textCol = selected ? ANTOS_COLOR_WHITE : ANTOS_COLOR_TEXT_DIM;

        AntBoy.Display.fillRoundRect(itemX, curY, itemW, itemH, 4, bg);
        AntBoy.Display.drawRoundRect(itemX, curY, itemW, itemH, 4, border);

        // Kursor panah seleksi
        if (selected) {
            AntBoy.Display.setTextSize(1);
            AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, bg);
            AntBoy.Display.setCursor(itemX + 8, curY + 11);
            AntBoy.Display.print(">");
        }

        // Nama Aplikasi
        AntBoy.Display.setTextSize(1);
        AntBoy.Display.setTextColor(textCol, bg);
        AntBoy.Display.setCursor(itemX + 22, curY + 11);
        AntBoy.Display.print(data.appList[i]);

        // Tag Status di kanan
        AntBoy.Display.setTextColor(data.themeColor, bg);
        int tagX = itemX + itemW - 60;
        AntBoy.Display.setCursor(tagX, curY + 11);
        AntBoy.Display.print("[SELECT]");
    }
}

void AntOS_LauncherClass::drawFooterGuide() {
    int fy = ANTBOY_SCREEN_HEIGHT - 22;
    AntBoy.Display.fillRect(0, fy, ANTBOY_SCREEN_WIDTH, 22, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.drawFastHLine(0, fy, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_BORDER_DIM);

    AntBoy.Display.setTextSize(1);
    if (_inSubMenu) {
        AntBoy.Display.drawCenteredText("D-Pad [^/v] Pilih | [A] Jalankan | [B] Kembali", fy + 7, ANTOS_COLOR_TEXT_DIM, 1);
    } else {
        AntBoy.Display.drawCenteredText("< > Geser Pilar | [A] Masuk | [MENU] Settings", fy + 7, ANTOS_COLOR_TEXT_DIM, 1);
    }
}

void AntOS_LauncherClass::render(bool forceRedraw) {
    if (!forceRedraw && !_needsRedraw) return;
    _needsRedraw = false;

    if (_inSubMenu) {
        drawSubMenu();
    } else {
        drawCarousel();
    }
    drawFooterGuide();
}

void AntOS_LauncherClass::update() {
    // Animasi periodik atau idle handling jika diperlukan
}

bool AntOS_LauncherClass::handleInput() {
    if (!_inSubMenu) {
        // --- NAVIGASI CAROUSEL MENU UTAMA ---
        if (AntBoy.Buttons.wasPressed(ANT_BTN_RIGHT)) {
            _currentPillar = (_currentPillar + 1) % ANTOS_PIL_COUNT;
            AntBoy.Audio.playTone(2637, 20);
            _needsRedraw = true;
            return true;
        }

        if (AntBoy.Buttons.wasPressed(ANT_BTN_LEFT)) {
            _currentPillar = (_currentPillar > 0) ? _currentPillar - 1 : ANTOS_PIL_COUNT - 1;
            AntBoy.Audio.playTone(2637, 20);
            _needsRedraw = true;
            return true;
        }

        if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
            _inSubMenu = true;
            _subItemIndex = 0;
            AntBoy.Audio.playConfirm();
            _needsRedraw = true;
            return true;
        }
    } else {
        // --- NAVIGASI SUB-MENU PILAR ---
        if (AntBoy.Buttons.wasPressed(ANT_BTN_UP)) {
            _subItemIndex = (_subItemIndex > 0) ? _subItemIndex - 1 : 3;
            AntBoy.Audio.playTone(2637, 15);
            _needsRedraw = true;
            return true;
        }

        if (AntBoy.Buttons.wasPressed(ANT_BTN_DOWN)) {
            _subItemIndex = (_subItemIndex + 1) % 4;
            AntBoy.Audio.playTone(2637, 15);
            _needsRedraw = true;
            return true;
        }

        if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
            // Placeholder peluncuran aplikasi (akan disambungkan di Fase 3, 4, 5, 6)
            AntBoy.Audio.playConfirm();
            AntBoy.Display.fillRoundRect(50, 90, 220, 50, 6, ANTOS_COLOR_BG_PANEL);
            AntBoy.Display.drawRoundRect(50, 90, 220, 50, 6, s_pillars[_currentPillar].themeColor);
            AntBoy.Display.setTextSize(1);
            AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
            AntBoy.Display.drawCenteredText("MEMULAI APLIKASI:", 102, ANTOS_COLOR_WHITE, 1);
            AntBoy.Display.drawCenteredText(s_pillars[_currentPillar].appList[_subItemIndex], 120, s_pillars[_currentPillar].themeColor, 1);
            delay(1200);
            _needsRedraw = true;
            return true;
        }

        if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
            _inSubMenu = false;
            AntBoy.Audio.playClick();
            _needsRedraw = true;
            return true;
        }
    }

    return false;
}
