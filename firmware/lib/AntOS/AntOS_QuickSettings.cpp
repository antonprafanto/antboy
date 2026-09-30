#include "AntOS_QuickSettings.h"

AntOS_QuickSettingsClass AntOS_QuickSettings;

void AntOS_QuickSettingsClass::begin() {
    _isOpen = false;
    _selectedIndex = 0;
}

void AntOS_QuickSettingsClass::open() {
    _isOpen = true;
    _selectedIndex = 0;
    AntBoy.Audio.playConfirm();
    render();
}

void AntOS_QuickSettingsClass::close() {
    _isOpen = false;
    AntOS_Settings.save(); // Simpan perubahan ke NVS
    AntBoy.Audio.playClick();
}

void AntOS_QuickSettingsClass::toggle() {
    if (_isOpen) {
        close();
    } else {
        open();
    }
}

void AntOS_QuickSettingsClass::drawItem(int x, int y, int w, int h, int index, const char* label, const char* value, bool selected) {
    uint16_t bg = selected ? ANTOS_COLOR_BG_CARD_ACTIVE : ANTOS_COLOR_BG_PANEL;
    uint16_t border = selected ? ANTOS_COLOR_CYAN : ANTOS_COLOR_BG_PANEL;
    uint16_t textCol = selected ? ANTOS_COLOR_WHITE : ANTOS_COLOR_TEXT_DIM;
    uint16_t valCol = selected ? ANTOS_COLOR_YELLOW : ANTOS_COLOR_CYAN;

    AntBoy.Display.fillRoundRect(x, y, w, h, 3, bg);
    if (selected) {
        AntBoy.Display.drawRoundRect(x, y, w, h, 3, border);
        // Indikator panah seleksi
        AntBoy.Display.setTextSize(1);
        AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, bg);
        AntBoy.Display.setCursor(x + 5, y + 6);
        AntBoy.Display.print(">");
    }

    // Label
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(textCol, bg);
    AntBoy.Display.setCursor(x + 16, y + 6);
    AntBoy.Display.print(label);

    // Value (Right-aligned inside item)
    AntBoy.Display.setTextColor(valCol, bg);
    int valX = x + w - (strlen(value) * 6) - 10;
    AntBoy.Display.setCursor(valX, y + 6);
    AntBoy.Display.print(value);
}

void AntOS_QuickSettingsClass::render() {
    if (!_isOpen) return;

    int mx = 30;
    int my = 26;
    int mw = 260;
    int mh = 188;

    // Modal Background & Border
    AntBoy.Display.fillRoundRect(mx, my, mw, mh, 8, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawRoundRect(mx, my, mw, mh, 8, ANTOS_COLOR_CYAN);
    AntBoy.Display.drawRoundRect(mx + 2, my + 2, mw - 4, mh - 4, 6, ANTOS_COLOR_BORDER_GLOW);

    // Header Modal
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("QUICK SETTINGS", my + 10, ANTOS_COLOR_YELLOW, 1);
    AntBoy.Display.drawFastHLine(mx + 12, my + 26, mw - 24, ANTOS_COLOR_BORDER_DIM);

    // Format Nilai Pengaturan
    char brightBuf[16];
    snprintf(brightBuf, sizeof(brightBuf), "< %d%% >", AntOS_Settings.brightness);

    char volBuf[16];
    snprintf(volBuf, sizeof(volBuf), "< %s >", AntBoy.Audio.getVolumeString());

    int itemX = mx + 10;
    int itemY = my + 34;
    int itemW = mw - 20;
    int itemH = 22;
    int spacing = 26;

    // Item 0: Kecerahan
    drawItem(itemX, itemY + (0 * spacing), itemW, itemH, 0, "Brightness", brightBuf, (_selectedIndex == 0));

    // Item 1: Volume
    drawItem(itemX, itemY + (1 * spacing), itemW, itemH, 1, "Audio Volume", volBuf, (_selectedIndex == 1));

    // Item 2: Sleep Mode
    drawItem(itemX, itemY + (2 * spacing), itemW, itemH, 2, "Sleep Mode", "[Press A]", (_selectedIndex == 2));

    // Item 3: Info Sistem
    drawItem(itemX, itemY + (3 * spacing), itemW, itemH, 3, "System Info", "240M HW:V1", (_selectedIndex == 3));

    // Item 4: Tutup Menu
    drawItem(itemX, itemY + (4 * spacing), itemW, itemH, 4, "Close Menu", "[B / MENU]", (_selectedIndex == 4));

    // Footer Panduan Modal
    AntBoy.Display.drawFastHLine(mx + 12, my + 164, mw - 24, ANTOS_COLOR_BORDER_DIM);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.drawCenteredText("D-Pad Nav | [A] Pilih | [B] Tutup", my + 172, ANTOS_COLOR_TEXT_MUTED, 1);
}

bool AntOS_QuickSettingsClass::handleInput() {
    if (!_isOpen) return false;

    // 1. Navigasi Atas / Bawah
    if (AntBoy.Buttons.wasPressed(ANT_BTN_UP)) {
        _selectedIndex = (_selectedIndex > 0) ? _selectedIndex - 1 : _menuItemCount - 1;
        AntBoy.Audio.playTone(2637, 20);
        render();
        return true;
    }

    if (AntBoy.Buttons.wasPressed(ANT_BTN_DOWN)) {
        _selectedIndex = (_selectedIndex + 1) % _menuItemCount;
        AntBoy.Audio.playTone(2637, 20);
        render();
        return true;
    }

    // 2. Aksi Tombol A / Kiri / Kanan
    if (AntBoy.Buttons.wasPressed(ANT_BTN_A) || 
        AntBoy.Buttons.wasPressed(ANT_BTN_RIGHT) || 
        AntBoy.Buttons.wasPressed(ANT_BTN_LEFT)) {
        
        switch (_selectedIndex) {
            case 0: // Brightness
                AntOS_Settings.cycleBrightness();
                AntBoy.Audio.playClick();
                render();
                return true;

            case 1: // Volume
                AntOS_Settings.cycleVolume();
                render();
                return true;

            case 2: // Sleep Mode
                AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
                AntBoy.Display.drawCenteredText("ENTERING DEEP SLEEP...", 100, ANTOS_COLOR_YELLOW, 1);
                AntBoy.Display.drawCenteredText("Press MENU or START to Wake", 130, ANTOS_COLOR_TEXT_DIM, 1);
                delay(800);
                AntBoy.Power.sleep(ANT_BTN_MENU);
                return true;

            case 3: // System Info
                AntBoy.Audio.playConfirm();
                render();
                return true;

            case 4: // Close Menu
                close();
                return true;
        }
    }

    // 3. Tombol B atau Tombol MENU menutup modal
    if (AntBoy.Buttons.wasPressed(ANT_BTN_B) || AntBoy.Buttons.wasPressed(ANT_BTN_MENU)) {
        close();
        return true;
    }

    return true; // Tangkap seluruh input saat modal aktif
}
