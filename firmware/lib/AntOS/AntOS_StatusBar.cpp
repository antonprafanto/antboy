#include "AntOS_StatusBar.h"

AntOS_StatusBarClass AntOS_StatusBar;

void AntOS_StatusBarClass::begin() {
    _lastRenderTime = 0;
    _toastActive = false;
    _lastSDState = false;
    _lastVol = 255;
    _lastSecs = 0xFFFF;
}

void AntOS_StatusBarClass::drawSDIcon(int x, int y, bool mounted) {
    uint16_t col = mounted ? ANTOS_COLOR_GREEN : ANTOS_COLOR_TEXT_MUTED;
    // Bentuk kartu MicroSD (lebar 10, tinggi 12)
    AntBoy.Display.drawRect(x, y, 9, 12, col);
    AntBoy.Display.drawFastHLine(x + 2, y + 2, 5, col);
    AntBoy.Display.drawFastVLine(x + 7, y + 2, 4, col);
    if (mounted) {
        AntBoy.Display.fillRect(x + 2, y + 4, 4, 5, col);
    } else {
        // Tanda silang kecil jika kartu tidak terpasang
        AntBoy.Display.drawLine(x + 2, y + 4, x + 6, y + 8, ANTOS_COLOR_RED);
        AntBoy.Display.drawLine(x + 2, y + 8, x + 6, y + 4, ANTOS_COLOR_RED);
    }
}

void AntOS_StatusBarClass::drawWifiIcon(int x, int y, bool active) {
    uint16_t col = active ? ANTOS_COLOR_CYAN : ANTOS_COLOR_TEXT_MUTED;
    // Titik pusat
    AntBoy.Display.drawPixel(x + 4, y + 9, col);
    // Arc bawah
    AntBoy.Display.drawPixel(x + 3, y + 7, col);
    AntBoy.Display.drawPixel(x + 4, y + 6, col);
    AntBoy.Display.drawPixel(x + 5, y + 7, col);
    // Arc atas
    AntBoy.Display.drawPixel(x + 1, y + 4, col);
    AntBoy.Display.drawPixel(x + 2, y + 3, col);
    AntBoy.Display.drawPixel(x + 3, y + 2, col);
    AntBoy.Display.drawPixel(x + 4, y + 2, col);
    AntBoy.Display.drawPixel(x + 5, y + 2, col);
    AntBoy.Display.drawPixel(x + 6, y + 3, col);
    AntBoy.Display.drawPixel(x + 7, y + 4, col);
}

void AntOS_StatusBarClass::drawVolumeIcon(int x, int y, uint8_t level) {
    uint16_t col = (level == 0) ? ANTOS_COLOR_RED : ANTOS_COLOR_YELLOW;
    // Speaker cone
    AntBoy.Display.fillRect(x, y + 3, 3, 5, col);
    AntBoy.Display.drawLine(x + 3, y + 3, x + 6, y, col);
    AntBoy.Display.drawLine(x + 3, y + 7, x + 6, y + 10, col);
    AntBoy.Display.drawFastVLine(x + 6, y, 11, col);

    if (level == 0) {
        // Tanda silang MUTE
        AntBoy.Display.drawLine(x + 8, y + 3, x + 12, y + 7, ANTOS_COLOR_RED);
        AntBoy.Display.drawLine(x + 8, y + 7, x + 12, y + 3, ANTOS_COLOR_RED);
    } else {
        // Bar gelombang suara
        if (level >= 1) AntBoy.Display.drawFastVLine(x + 8, y + 3, 5, col);
        if (level >= 3) AntBoy.Display.drawFastVLine(x + 10, y + 1, 9, col);
    }
}

void AntOS_StatusBarClass::render(bool forceRedraw) {
    bool currentSD = AntBoy.SD.isMounted();
    uint8_t currentVol = (uint8_t)AntBoy.Audio.getVolume();
    uint32_t totalSecs = millis() / 1000;
    uint16_t mins = (totalSecs / 60) % 100;
    uint16_t secs = totalSecs % 60;

    if (!forceRedraw && currentSD == _lastSDState && currentVol == _lastVol && secs == _lastSecs) {
        return;
    }

    _lastSDState = currentSD;
    _lastVol = currentVol;
    _lastSecs = secs;

    // Background bar
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 22, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.drawFastHLine(0, 22, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_CYAN);

    // Left Badge: "AntOS v1.0"
    AntBoy.Display.fillRoundRect(6, 3, 68, 16, 3, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(6, 3, 68, 16, 3, ANTOS_COLOR_BORDER_GLOW);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(12, 7);
    AntBoy.Display.print("AntOS");
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.print(" 1.0");

    // Right Icons
    // 1. SD Card (X: 172, Y: 5)
    drawSDIcon(172, 5, currentSD);

    // 2. Wi-Fi (X: 192, Y: 5)
    drawWifiIcon(192, 5, true);

    // 3. Volume (X: 212, Y: 5)
    drawVolumeIcon(212, 5, currentVol);

    // 4. Uptime Clock (X: 236, Y: 7)
    char timeStr[10];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", mins, secs);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_DARK);
    AntBoy.Display.setCursor(236, 7);
    AntBoy.Display.print(timeStr);

    // 5. Battery Icon (X: 284, Y: 6)
    drawBatteryIcon(284, 6);
}

void AntOS_StatusBarClass::drawBatteryIcon(int x, int y) {
    // Battery shell (lebar 24, tinggi 10)
    AntBoy.Display.drawRoundRect(x, y, 22, 10, 2, ANTOS_COLOR_BORDER_GLOW);
    AntBoy.Display.fillRect(x + 22, y + 2, 2, 6, ANTOS_COLOR_BORDER_GLOW); // Terminal (+)
    // USB / Battery indicator bar
    AntBoy.Display.fillRoundRect(x + 2, y + 2, 18, 6, 1, ANTOS_COLOR_GREEN);
    // Simbol lightning / charge kecil di tengah
    AntBoy.Display.drawPixel(x + 10, y + 3, ANTOS_COLOR_YELLOW);
    AntBoy.Display.drawPixel(x + 9, y + 4, ANTOS_COLOR_YELLOW);
    AntBoy.Display.drawPixel(x + 10, y + 5, ANTOS_COLOR_YELLOW);
    AntBoy.Display.drawPixel(x + 11, y + 6, ANTOS_COLOR_YELLOW);
}

void AntOS_StatusBarClass::triggerVolumeOSD(const char* volStr, uint8_t level) {
    _toastActive = true;
    _toastDismissTime = millis() + 1500; // Hilang setelah 1.5 detik
    strncpy(_toastVolStr, volStr, sizeof(_toastVolStr) - 1);
    _toastVolLevel = level;
    renderToast();
}

void AntOS_StatusBarClass::renderToast() {
    if (!_toastActive) return;

    // Toast Container (Tengah layar bagian atas)
    int tx = 80;
    int ty = 28;
    int tw = 160;
    int th = 38;

    AntBoy.Display.fillRoundRect(tx, ty, tw, th, 6, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(tx, ty, tw, th, 6, ANTOS_COLOR_CYAN);
    AntBoy.Display.drawRoundRect(tx + 1, ty + 1, tw - 2, th - 2, 5, ANTOS_COLOR_BORDER_GLOW);

    // Label Volume
    char buf[32];
    snprintf(buf, sizeof(buf), "VOLUME: %s", _toastVolStr);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(tx + 12, ty + 7);
    AntBoy.Display.print(buf);

    // 4 Bar Segmen Volume
    int barX = tx + 12;
    int barY = ty + 20;
    int barW = 30;
    int barH = 10;
    int gap = 5;

    for (int i = 1; i <= 4; i++) {
        uint16_t bCol = (_toastVolLevel >= i) ? ANTOS_COLOR_CYAN : ANTOS_COLOR_BG_CARD;
        AntBoy.Display.fillRoundRect(barX + ((i - 1) * (barW + gap)), barY, barW, barH, 2, bCol);
        AntBoy.Display.drawRoundRect(barX + ((i - 1) * (barW + gap)), barY, barW, barH, 2, ANTOS_COLOR_BORDER_DIM);
    }
}

void AntOS_StatusBarClass::update() {
    if (_toastActive && millis() > _toastDismissTime) {
        _toastActive = false;
        _toastJustClosed = true;
    }
}

bool AntOS_StatusBarClass::checkToastClosed() {
    if (_toastJustClosed) {
        _toastJustClosed = false;
        return true;
    }
    return false;
}
