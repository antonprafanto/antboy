#include "BreakoutGame.h"
#include <Preferences.h>

BreakoutGameClass BreakoutGame;

static const uint16_t BRICK_COLORS[5] = {
    ANTOS_COLOR_RED,
    0xFD20, // Orange
    ANTOS_COLOR_YELLOW,
    ANTOS_COLOR_GREEN,
    ANTOS_COLOR_CYAN
};

static const int BRICK_POINTS[5] = { 50, 40, 30, 20, 10 };

void BreakoutGameClass::loadHighScore() {
    Preferences prefs;
    prefs.begin("antboy_breakout", true);
    highScore = prefs.getInt("hi", 0);
    prefs.end();
}

void BreakoutGameClass::saveHighScore() {
    if (score > highScore) {
        highScore = score;
        Preferences prefs;
        prefs.begin("antboy_breakout", false);
        prefs.putInt("hi", highScore);
        prefs.end();
    }
}

void BreakoutGameClass::initBricks() {
    bricksLeft = BRICK_ROWS * BRICK_COLS;
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            bricks[r][c] = true;
        }
    }
}

void BreakoutGameClass::resetBall() {
    ballAttached = true;
    paddleX = (ANTBOY_SCREEN_WIDTH - paddleW) / 2;
    ballX = paddleX + (paddleW / 2);
    ballY = 208;
    ballVX = 2.4;
    ballVY = -2.8;
}

void BreakoutGameClass::resetGame() {
    score = 0;
    lives = 3;
    paddleW = 44;
    gameOver = false;
    gameWon = false;
    initBricks();
    resetBall();
}

void BreakoutGameClass::updateLogic() {
    if (ballAttached) {
        ballX = paddleX + (paddleW / 2);
        ballY = 208;
        return;
    }

    ballX += ballVX;
    ballY += ballVY;

    // Pantulan dinding kiri & kanan
    if (ballX <= 4) {
        ballX = 4;
        ballVX = -ballVX;
        AntBoy.Audio.playTone(1200, 8);
    } else if (ballX >= ANTBOY_SCREEN_WIDTH - 8) {
        ballX = ANTBOY_SCREEN_WIDTH - 8;
        ballVX = -ballVX;
        AntBoy.Audio.playTone(1200, 8);
    }

    // Pantulan atap
    if (ballY <= 26) {
        ballY = 26;
        ballVY = -ballVY;
        AntBoy.Audio.playTone(1200, 8);
    }

    // Pantulan Paddle
    if (ballY >= 210 && ballY <= 216 && ballVX != 0) {
        if (ballX >= paddleX - 4 && ballX <= paddleX + paddleW + 4) {
            ballY = 210;
            // Hit angle calculation
            float hitPos = (ballX - (paddleX + (paddleW / 2))) / (paddleW / 2);
            ballVX = hitPos * 3.5;
            ballVY = -sqrt(16.0 - (ballVX * ballVX)); // Pertahankan total kecepatan konstan ~4 px/frame
            if (ballVY > -2.0) ballVY = -2.0;
            AntBoy.Audio.playTone(1600, 12);
        }
    }

    // Cek Bola Jatuh ke Bawah
    if (ballY > 230) {
        lives--;
        AntBoy.Audio.playError();
        if (lives <= 0) {
            gameOver = true;
            saveHighScore();
        } else {
            resetBall();
        }
        return;
    }

    // Cek Tabrakan Bata (Bricks)
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            if (bricks[r][c]) {
                int bx = BRICK_START_X + (c * (BRICK_W + 3));
                int by = BRICK_START_Y + (r * (BRICK_H + 4));

                if (ballX >= bx - 3 && ballX <= bx + BRICK_W + 3 &&
                    ballY >= by - 3 && ballY <= by + BRICK_H + 3) {
                    bricks[r][c] = false;
                    bricksLeft--;
                    score += BRICK_POINTS[r];
                    ballVY = -ballVY;
                    AntBoy.Audio.playTone(2000 + (r * 150), 12);

                    if (bricksLeft <= 0) {
                        gameWon = true;
                        gameOver = true;
                        AntBoy.Audio.playConfirm();
                        saveHighScore();
                    }
                    return;
                }
            }
        }
    }
}

void BreakoutGameClass::render() {
    // 1. Top HUD
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 22, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 22, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_CYAN);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_CYAN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(10, 7);
    AntBoy.Display.printf("BREAKOUT  SCORE: %04d", score);

    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(170, 7);
    AntBoy.Display.printf("HI: %04d", highScore);

    AntBoy.Display.setTextColor(ANTOS_COLOR_GREEN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(260, 7);
    AntBoy.Display.printf("BALLS: %d", lives);

    // 2. Play Area
    AntBoy.Display.fillRect(0, 23, ANTBOY_SCREEN_WIDTH, 217, ANTOS_COLOR_BG_DARK);

    // Gambar Seluruh Bata
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            if (bricks[r][c]) {
                int bx = BRICK_START_X + (c * (BRICK_W + 3));
                int by = BRICK_START_Y + (r * (BRICK_H + 4));
                AntBoy.Display.fillRoundRect(bx, by, BRICK_W, BRICK_H, 2, BRICK_COLORS[r]);
                AntBoy.Display.drawFastHLine(bx + 1, by + 1, BRICK_W - 2, ANTOS_COLOR_WHITE); // Bevel highlight
            }
        }
    }

    // Paddle
    AntBoy.Display.fillRoundRect((int)paddleX, 214, paddleW, 8, 3, ANTOS_COLOR_CYAN);
    AntBoy.Display.drawFastHLine((int)paddleX + 2, 215, paddleW - 4, ANTOS_COLOR_WHITE);

    // Bola
    AntBoy.Display.fillCircle((int)ballX, (int)ballY, 3, ANTOS_COLOR_YELLOW);

    if (ballAttached) {
        AntBoy.Display.setTextSize(1);
        AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_DARK);
        AntBoy.Display.drawCenteredText("Tekan [A] untuk Lepas Bola", 160, ANTOS_COLOR_WHITE, 1);
    }

    if (gameOver) {
        int modalW = 200;
        int modalH = 80;
        int mx = (ANTBOY_SCREEN_WIDTH - modalW) / 2;
        int my = (ANTBOY_SCREEN_HEIGHT - modalH) / 2;
        uint16_t borderCol = gameWon ? ANTOS_COLOR_GREEN : ANTOS_COLOR_RED;

        AntBoy.Display.fillRoundRect(mx, my, modalW, modalH, 6, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawRoundRect(mx, my, modalW, modalH, 6, borderCol);

        AntBoy.Display.setTextSize(2);
        AntBoy.Display.setTextColor(borderCol, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawCenteredText(gameWon ? "VICTORY!" : "GAME OVER!", my + 14, borderCol, 1);

        AntBoy.Display.setTextSize(1);
        AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
        AntBoy.Display.drawCenteredText("[A] Main Lagi  |  [B] Keluar", my + 48, ANTOS_COLOR_WHITE, 1);
    }
}

void BreakoutGameClass::run() {
    loadHighScore();
    resetGame();
    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
    AntBoy.Audio.playTone(1500, 30);

    bool running = true;

    while (running) {
        AntBoy.update();

        if (AntBoy.checkExitShortcut()) {
            break;
        }

        if (!gameOver) {
            float speed = AntBoy.Buttons.isPressed(ANT_BTN_B) ? 7.0 : 4.5;
            if (AntBoy.Buttons.isPressed(ANT_BTN_LEFT)) {
                paddleX -= speed;
                if (paddleX < 4) paddleX = 4;
            }
            if (AntBoy.Buttons.isPressed(ANT_BTN_RIGHT)) {
                paddleX += speed;
                if (paddleX > ANTBOY_SCREEN_WIDTH - paddleW - 4) paddleX = ANTBOY_SCREEN_WIDTH - paddleW - 4;
            }
            if (ballAttached && AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
                ballAttached = false;
                AntBoy.Audio.playTone(1800, 15);
            }
            updateLogic();
        } else {
            if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
                resetGame();
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
                running = false;
                break;
            }
        }

        render();
        delay(20);
    }

    saveHighScore();
}
