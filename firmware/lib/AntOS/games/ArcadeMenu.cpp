#include "ArcadeMenu.h"

ArcadeMenuClass ArcadeMenu;

struct ArcadeGameEntry {
    const char* title;
    const char* desc;
    uint16_t color;
    const char* genre;
};

static const ArcadeGameEntry ARCADE_GAMES[4] = {
    { "SNAKE RETRO",     "Classic arcade snake with NVS high score", ANTOS_COLOR_GREEN,   "CLASSIC" },
    { "TETRIS POCKET",   "10x20 block stacking & line clear SFX",   ANTOS_COLOR_CYAN,    "PUZZLE" },
    { "SPACE INVADERS",  "Alien swarm defense & laser cannon",      ANTOS_COLOR_MAGENTA, "SHOOTER" },
    { "BREAKOUT / PONG", "Paddle ball angle physics & brick smash", ANTOS_COLOR_YELLOW,  "ACTION" }
};

void ArcadeMenuClass::render() {
    // Header
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 26, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 26, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_PIL_GAMING);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(10, 9);
    AntBoy.Display.print("< [B] BACK");

    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("NATIVE 8-BIT ARCADE", 9, ANTOS_COLOR_WHITE, 1);

    // List of 4 games
    int startY = 36;
    int itemH = 40;
    int spacing = 44;
    int itemW = 296;
    int itemX = 12;

    for (int i = 0; i < 4; i++) {
        bool selected = (selectedIndex == i);
        int curY = startY + (i * spacing);
        uint16_t bg = selected ? ANTOS_COLOR_BG_CARD_ACTIVE : ANTOS_COLOR_BG_PANEL;
        uint16_t border = selected ? ARCADE_GAMES[i].color : ANTOS_COLOR_BORDER_DIM;

        AntBoy.Display.fillRoundRect(itemX, curY, itemW, itemH, 4, bg);
        AntBoy.Display.drawRoundRect(itemX, curY, itemW, itemH, 4, border);

        // Icon indicator
        if (selected) {
            AntBoy.Display.fillRoundRect(itemX + 8, curY + 8, 24, 24, 3, ARCADE_GAMES[i].color);
            AntBoy.Display.setTextSize(1);
            AntBoy.Display.setTextColor(ANTOS_COLOR_BG_DARK, ARCADE_GAMES[i].color);
            AntBoy.Display.setCursor(itemX + 17, curY + 16);
            AntBoy.Display.print(">");
        } else {
            AntBoy.Display.drawRoundRect(itemX + 8, curY + 8, 24, 24, 3, ANTOS_COLOR_BORDER_DIM);
            AntBoy.Display.setTextSize(1);
            AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, bg);
            AntBoy.Display.setCursor(itemX + 17, curY + 16);
            AntBoy.Display.printf("%d", i + 1);
        }

        // Title
        AntBoy.Display.setTextSize(1);
        AntBoy.Display.setTextColor(selected ? ANTOS_COLOR_WHITE : ANTOS_COLOR_TEXT_DIM, bg);
        AntBoy.Display.setCursor(itemX + 40, curY + 9);
        AntBoy.Display.print(ARCADE_GAMES[i].title);

        // Subtitle / Description
        AntBoy.Display.setTextColor(selected ? ARCADE_GAMES[i].color : 0x7BEF, bg);
        AntBoy.Display.setCursor(itemX + 40, curY + 23);
        AntBoy.Display.print(ARCADE_GAMES[i].desc);

        // Badge
        int badgeW = 55;
        int badgeX = itemX + itemW - badgeW - 8;
        int badgeY = curY + 12;
        AntBoy.Display.fillRoundRect(badgeX, badgeY, badgeW, 16, 3, ANTOS_COLOR_BG_DARK);
        AntBoy.Display.drawRoundRect(badgeX, badgeY, badgeW, 16, 3, ARCADE_GAMES[i].color);
        AntBoy.Display.setTextColor(ARCADE_GAMES[i].color, ANTOS_COLOR_BG_DARK);
        AntBoy.Display.setCursor(badgeX + 6, badgeY + 4);
        AntBoy.Display.print(ARCADE_GAMES[i].genre);
    }

    // Footer
    int fy = ANTBOY_SCREEN_HEIGHT - 20;
    AntBoy.Display.fillRect(0, fy, ANTBOY_SCREEN_WIDTH, 20, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, fy, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_BORDER_DIM);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("D-Pad [^/v] Pilih  |  [A] Mulai Game  |  [B] Kembali", fy + 6, ANTOS_COLOR_TEXT_DIM, 1);
}

void ArcadeMenuClass::run() {
    selectedIndex = 0;
    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
    render();

    bool inMenu = true;

    while (inMenu) {
        AntBoy.update();

        if (AntBoy.checkExitShortcut()) {
            break;
        }

        if (AntBoy.Buttons.wasPressed(ANT_BTN_UP)) {
            selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : 3;
            AntBoy.Audio.playTone(2637, 15);
            render();
        } else if (AntBoy.Buttons.wasPressed(ANT_BTN_DOWN)) {
            selectedIndex = (selectedIndex + 1) % 4;
            AntBoy.Audio.playTone(2637, 15);
            render();
        } else if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
            AntBoy.Audio.playConfirm();
            switch (selectedIndex) {
                case 0: SnakeGame.run(); break;
                case 1: TetrisGame.run(); break;
                case 2: SpaceInvadersGame.run(); break;
                case 3: BreakoutGame.run(); break;
            }
            AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
            render();
        } else if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
            AntBoy.Audio.playClick();
            inMenu = false;
        }

        delay(15);
    }
}
