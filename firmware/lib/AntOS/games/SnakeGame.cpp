#include "SnakeGame.h"
#include <Preferences.h>

SnakeGameClass SnakeGame;

void SnakeGameClass::loadHighScore() {
    Preferences prefs;
    prefs.begin("antboy_snake", true);
    highScore = prefs.getInt("hi", 0);
    prefs.end();
}

void SnakeGameClass::saveHighScore() {
    if (score > highScore) {
        highScore = score;
        Preferences prefs;
        prefs.begin("antboy_snake", false);
        prefs.putInt("hi", highScore);
        prefs.end();
    }
}

void SnakeGameClass::spawnFood() {
    bool onSnake;
    do {
        onSnake = false;
        food.x = random(0, GRID_W);
        food.y = random(0, GRID_H);
        for (int i = 0; i < snakeLen; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}

void SnakeGameClass::resetGame() {
    snakeLen = 4;
    int startX = GRID_W / 2;
    int startY = GRID_H / 2;
    for (int i = 0; i < snakeLen; i++) {
        snake[i].x = startX - i;
        snake[i].y = startY;
    }
    dirX = 1;
    dirY = 0;
    nextDirX = 1;
    nextDirY = 0;
    score = 0;
    gameOver = false;
    spawnFood();
}

void SnakeGameClass::updateLogic() {
    dirX = nextDirX;
    dirY = nextDirY;

    Point newHead = { (int8_t)(snake[0].x + dirX), (int8_t)(snake[0].y + dirY) };

    // Tabrakan dengan dinding
    if (newHead.x < 0 || newHead.x >= GRID_W || newHead.y < 0 || newHead.y >= GRID_H) {
        gameOver = true;
        AntBoy.Audio.playError();
        saveHighScore();
        return;
    }

    // Tabrakan dengan badan sendiri
    for (int i = 0; i < snakeLen; i++) {
        if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
            gameOver = true;
            AntBoy.Audio.playError();
            saveHighScore();
            return;
        }
    }

    // Geser badan
    for (int i = snakeLen - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = newHead;

    // Cek makan makanan
    if (newHead.x == food.x && newHead.y == food.y) {
        score += 10;
        AntBoy.Audio.playTone(2093, 20); // C7 chime
        if (snakeLen < 195) {
            snake[snakeLen] = snake[snakeLen - 1];
            snakeLen++;
        }
        spawnFood();
    }
}

void SnakeGameClass::render() {
    // Top Score Bar
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 22, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 22, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_GREEN);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_GREEN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(12, 7);
    AntBoy.Display.printf("SNAKE RETRO  SCORE: %04d", score);

    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(205, 7);
    AntBoy.Display.printf("HIGH: %04d", highScore);

    // Arena border
    AntBoy.Display.drawRect(OFFSET_X - 1, OFFSET_Y - 1, (GRID_W * CELL_SIZE) + 2, (GRID_H * CELL_SIZE) + 2, ANTOS_COLOR_BORDER_DIM);
    AntBoy.Display.fillRect(OFFSET_X, OFFSET_Y, GRID_W * CELL_SIZE, GRID_H * CELL_SIZE, ANTOS_COLOR_BG_DARK);

    // Gambar makanan (Apple neon merah)
    int fx = OFFSET_X + (food.x * CELL_SIZE);
    int fy = OFFSET_Y + (food.y * CELL_SIZE);
    AntBoy.Display.fillRoundRect(fx + 2, fy + 2, CELL_SIZE - 4, CELL_SIZE - 4, 3, ANTOS_COLOR_RED);
    AntBoy.Display.drawPixel(fx + 4, fy + 4, ANTOS_COLOR_WHITE); // Highlight

    // Gambar ular
    for (int i = 0; i < snakeLen; i++) {
        int sx = OFFSET_X + (snake[i].x * CELL_SIZE);
        int sy = OFFSET_Y + (snake[i].y * CELL_SIZE);
        if (i == 0) {
            // Kepala
            AntBoy.Display.fillRoundRect(sx + 1, sy + 1, CELL_SIZE - 2, CELL_SIZE - 2, 3, ANTOS_COLOR_GREEN);
            // Mata
            AntBoy.Display.drawPixel(sx + 4, sy + 4, ANTOS_COLOR_BG_DARK);
            AntBoy.Display.drawPixel(sx + 10, sy + 4, ANTOS_COLOR_BG_DARK);
        } else {
            // Tubuh
            uint16_t bodyColor = (i % 2 == 0) ? 0x0664 : 0x0523; // Neon cyber green shading
            AntBoy.Display.fillRoundRect(sx + 2, sy + 2, CELL_SIZE - 4, CELL_SIZE - 4, 2, bodyColor);
        }
    }

    if (gameOver) {
        int modalW = 200;
        int modalH = 80;
        int mx = (ANTBOY_SCREEN_WIDTH - modalW) / 2;
        int my = (ANTBOY_SCREEN_HEIGHT - modalH) / 2;
        AntBoy.Display.fillRoundRect(mx, my, modalW, modalH, 6, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawRoundRect(mx, my, modalW, modalH, 6, ANTOS_COLOR_RED);

        AntBoy.Display.setTextSize(2);
        AntBoy.Display.setTextColor(ANTOS_COLOR_RED, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawCenteredText("GAME OVER!", my + 14, ANTOS_COLOR_RED, 1);

        AntBoy.Display.setTextSize(1);
        AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawCenteredText("[A] Main Lagi  |  [B] Keluar", my + 48, ANTOS_COLOR_WHITE, 1);
    }
}

void SnakeGameClass::run() {
    loadHighScore();
    resetGame();
    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
    AntBoy.Audio.playTone(1760, 40); // A6 beep

    uint32_t lastStep = millis();
    bool running = true;

    while (running) {
        AntBoy.update();

        // Cek Exit shortcut (SELECT + START)
        if (AntBoy.checkExitShortcut()) {
            break;
        }

        // Input
        if (!gameOver) {
            if (AntBoy.Buttons.wasPressed(ANT_BTN_UP) && dirY == 0) {
                nextDirX = 0; nextDirY = -1;
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_DOWN) && dirY == 0) {
                nextDirX = 0; nextDirY = 1;
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_LEFT) && dirX == 0) {
                nextDirX = -1; nextDirY = 0;
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_RIGHT) && dirX == 0) {
                nextDirX = 1; nextDirY = 0;
            }
        } else {
            if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
                resetGame();
                lastStep = millis();
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
                running = false;
                break;
            }
        }

        // Kecepatan adaptif berdasarkan skor
        uint32_t interval = (score < 50) ? 140 : ((score < 150) ? 110 : ((score < 300) ? 85 : 65));

        if (!gameOver && (millis() - lastStep >= interval)) {
            lastStep = millis();
            updateLogic();
            render();
        } else if (gameOver) {
            render();
            delay(30);
        }

        delay(10);
    }

    saveHighScore();
}
