#include "SpaceInvadersGame.h"
#include <Preferences.h>

SpaceInvadersGameClass SpaceInvadersGame;

void SpaceInvadersGameClass::loadHighScore() {
    Preferences prefs;
    prefs.begin("antboy_invaders", true);
    highScore = prefs.getInt("hi", 0);
    prefs.end();
}

void SpaceInvadersGameClass::saveHighScore() {
    if (score > highScore) {
        highScore = score;
        Preferences prefs;
        prefs.begin("antboy_invaders", false);
        prefs.putInt("hi", highScore);
        prefs.end();
    }
}

void SpaceInvadersGameClass::resetGame() {
    score = 0;
    wave = 1;
    playerLives = 3;
    playerX = ANTBOY_SCREEN_WIDTH / 2;
    playerBullet.active = false;
    for (int i = 0; i < MAX_BOMBS; i++) {
        alienBombs[i].active = false;
    }
    ufoActive = false;
    lastUfoSpawn = millis();
    gameOver = false;
    initWave();
}

void SpaceInvadersGameClass::initWave() {
    livingAliens = ALIEN_ROWS * ALIEN_COLS;
    alienDir = 1;
    alienStepTimer = 0;
    alienAnimFrame = 0;

    int startX = 35;
    int startY = 38;
    int spacingX = 40;
    int spacingY = 22;

    for (int r = 0; r < ALIEN_ROWS; r++) {
        for (int c = 0; c < ALIEN_COLS; c++) {
            aliens[r][c].x = startX + (c * spacingX);
            aliens[r][c].y = startY + (r * spacingY);
            aliens[r][c].alive = true;
            aliens[r][c].type = r;
        }
    }
}

void SpaceInvadersGameClass::updateAliens() {
    // Kecepatan alien meningkat seiring berkurangnya alien
    alienStepTimer++;
    int threshold = (livingAliens > 15) ? 12 : ((livingAliens > 5) ? 7 : 3);
    if (alienStepTimer < threshold) return;
    alienStepTimer = 0;
    alienAnimFrame = !alienAnimFrame;

    // Langkah suara alien
    static const uint16_t marchTones[4] = { 220, 207, 196, 185 };
    static int toneIdx = 0;
    AntBoy.Audio.playTone(marchTones[toneIdx], 8);
    toneIdx = (toneIdx + 1) % 4;

    bool hitEdge = false;
    for (int r = 0; r < ALIEN_ROWS; r++) {
        for (int c = 0; c < ALIEN_COLS; c++) {
            if (aliens[r][c].alive) {
                aliens[r][c].x += (alienDir * 6);
                if (aliens[r][c].x < 15 || aliens[r][c].x > ANTBOY_SCREEN_WIDTH - 35) {
                    hitEdge = true;
                }
                if (aliens[r][c].y >= 195) {
                    gameOver = true;
                    AntBoy.Audio.playError();
                    saveHighScore();
                    return;
                }
            }
        }
    }

    if (hitEdge) {
        alienDir = -alienDir;
        for (int r = 0; r < ALIEN_ROWS; r++) {
            for (int c = 0; c < ALIEN_COLS; c++) {
                if (aliens[r][c].alive) {
                    aliens[r][c].y += 8;
                }
            }
        }
    }

    // Alien menembak bom secara acak
    if (random(0, 100) < 35) {
        for (int i = 0; i < MAX_BOMBS; i++) {
            if (!alienBombs[i].active) {
                // Pilih alien yang masih hidup di baris terbawah
                int randCol = random(0, ALIEN_COLS);
                for (int r = ALIEN_ROWS - 1; r >= 0; r--) {
                    if (aliens[r][randCol].alive) {
                        alienBombs[i].x = aliens[r][randCol].x + 8;
                        alienBombs[i].y = aliens[r][randCol].y + 12;
                        alienBombs[i].active = true;
                        break;
                    }
                }
                break;
            }
        }
    }
}

void SpaceInvadersGameClass::updateBullets() {
    // Update Peluru Player
    if (playerBullet.active) {
        playerBullet.y -= 7;
        if (playerBullet.y < 24) {
            playerBullet.active = false;
        } else {
            // Cek tabrakan dengan UFO
            if (ufoActive && playerBullet.y <= 36 && playerBullet.x >= ufoX && playerBullet.x <= ufoX + 24) {
                score += 150;
                ufoActive = false;
                playerBullet.active = false;
                AntBoy.Audio.playTone(2400, 30);
            }

            // Cek tabrakan dengan Alien
            for (int r = 0; r < ALIEN_ROWS; r++) {
                for (int c = 0; c < ALIEN_COLS; c++) {
                    if (aliens[r][c].alive) {
                        if (playerBullet.x >= aliens[r][c].x && playerBullet.x <= aliens[r][c].x + 18 &&
                            playerBullet.y >= aliens[r][c].y && playerBullet.y <= aliens[r][c].y + 14) {
                            aliens[r][c].alive = false;
                            playerBullet.active = false;
                            livingAliens--;
                            score += (4 - r) * 10;
                            AntBoy.Audio.playTone(1600, 15);

                            if (livingAliens <= 0) {
                                wave++;
                                AntBoy.Audio.playConfirm();
                                initWave();
                            }
                            return;
                        }
                    }
                }
            }
        }
    }

    // Update Bom Alien
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (alienBombs[i].active) {
            alienBombs[i].y += 4;
            if (alienBombs[i].y > 220) {
                alienBombs[i].active = false;
            } else if (alienBombs[i].y >= 204 && alienBombs[i].x >= playerX - 10 && alienBombs[i].x <= playerX + 10) {
                alienBombs[i].active = false;
                playerLives--;
                AntBoy.Audio.playError();
                if (playerLives <= 0) {
                    gameOver = true;
                    saveHighScore();
                }
            }
        }
    }

    // Update UFO
    if (ufoActive) {
        ufoX += 2;
        if (ufoX > ANTBOY_SCREEN_WIDTH) {
            ufoActive = false;
        }
    } else if (millis() - lastUfoSpawn >= 20000) {
        lastUfoSpawn = millis();
        ufoActive = true;
        ufoX = -20;
    }
}

void SpaceInvadersGameClass::render() {
    // 1. Top HUD Bar
    AntBoy.Display.fillRect(0, 0, ANTBOY_SCREEN_WIDTH, 22, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawFastHLine(0, 22, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_PIL_GAMING);

    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_WHITE, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(10, 7);
    AntBoy.Display.printf("SCORE: %04d", score);

    AntBoy.Display.setTextColor(ANTOS_COLOR_YELLOW, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(120, 7);
    AntBoy.Display.printf("HI: %04d", highScore);

    AntBoy.Display.setTextColor(ANTOS_COLOR_GREEN, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setCursor(210, 7);
    AntBoy.Display.printf("LIVES: %d  W:%d", playerLives, wave);

    // 2. Play Area Background
    AntBoy.Display.fillRect(0, 23, ANTBOY_SCREEN_WIDTH, 195, ANTOS_COLOR_BG_DARK);

    // Bintang latar belakang sederhana
    AntBoy.Display.drawPixel(40, 50, 0x7BEF);
    AntBoy.Display.drawPixel(180, 80, 0x7BEF);
    AntBoy.Display.drawPixel(280, 140, 0x7BEF);

    // UFO Merah
    if (ufoActive) {
        AntBoy.Display.fillRoundRect(ufoX, 26, 22, 8, 3, ANTOS_COLOR_RED);
        AntBoy.Display.drawFastHLine(ufoX + 3, 30, 16, ANTOS_COLOR_WHITE);
    }

    // Alien Swarm
    for (int r = 0; r < ALIEN_ROWS; r++) {
        for (int c = 0; c < ALIEN_COLS; c++) {
            if (aliens[r][c].alive) {
                int ax = aliens[r][c].x;
                int ay = aliens[r][c].y;
                uint16_t col = (r == 0) ? ANTOS_COLOR_MAGENTA : ((r == 1) ? ANTOS_COLOR_CYAN : ANTOS_COLOR_GREEN);

                AntBoy.Display.fillRoundRect(ax + 2, ay + 2, 14, 10, 2, col);
                // Mata
                AntBoy.Display.drawPixel(ax + 5, ay + 5, ANTOS_COLOR_BG_DARK);
                AntBoy.Display.drawPixel(ax + 11, ay + 5, ANTOS_COLOR_BG_DARK);
                // Antena / Kaki animasi
                if (alienAnimFrame) {
                    AntBoy.Display.drawFastVLine(ax + 1, ay, 3, col);
                    AntBoy.Display.drawFastVLine(ax + 16, ay, 3, col);
                } else {
                    AntBoy.Display.drawFastVLine(ax + 1, ay + 9, 3, col);
                    AntBoy.Display.drawFastVLine(ax + 16, ay + 9, 3, col);
                }
            }
        }
    }

    // Peluru Player
    if (playerBullet.active) {
        AntBoy.Display.drawFastVLine(playerBullet.x, playerBullet.y, 6, ANTOS_COLOR_YELLOW);
        AntBoy.Display.drawFastVLine(playerBullet.x + 1, playerBullet.y, 6, ANTOS_COLOR_WHITE);
    }

    // Bom Alien
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (alienBombs[i].active) {
            AntBoy.Display.drawFastVLine(alienBombs[i].x, alienBombs[i].y, 5, ANTOS_COLOR_RED);
        }
    }

    // Ground Line
    AntBoy.Display.drawFastHLine(0, 218, ANTBOY_SCREEN_WIDTH, ANTOS_COLOR_GREEN);

    // Player Cannon
    AntBoy.Display.fillRoundRect(playerX - 10, 206, 20, 10, 2, ANTOS_COLOR_GREEN);
    AntBoy.Display.fillRect(playerX - 2, 201, 4, 6, ANTOS_COLOR_GREEN);

    // Footer Controls
    int fy = ANTBOY_SCREEN_HEIGHT - 20;
    AntBoy.Display.fillRect(0, fy, ANTBOY_SCREEN_WIDTH, 20, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.setTextSize(1);
    AntBoy.Display.setTextColor(ANTOS_COLOR_TEXT_DIM, ANTOS_COLOR_BG_PANEL);
    AntBoy.Display.drawCenteredText("< > Gerak | [A] Tembak Laser | [B] Keluar", fy + 6, ANTOS_COLOR_TEXT_DIM, 1);

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

void SpaceInvadersGameClass::run() {
    loadHighScore();
    resetGame();
    AntBoy.Display.fillScreen(ANTOS_COLOR_BG_DARK);
    AntBoy.Audio.playTone(1000, 30);

    bool running = true;

    while (running) {
        AntBoy.update();

        if (AntBoy.checkExitShortcut()) {
            break;
        }

        if (!gameOver) {
            if (AntBoy.Buttons.isPressed(ANT_BTN_LEFT)) {
                if (playerX > 15) playerX -= 4;
            }
            if (AntBoy.Buttons.isPressed(ANT_BTN_RIGHT)) {
                if (playerX < ANTBOY_SCREEN_WIDTH - 15) playerX += 4;
            }
            if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
                if (!playerBullet.active) {
                    playerBullet.x = playerX;
                    playerBullet.y = 200;
                    playerBullet.active = true;
                    AntBoy.Audio.playTone(1800, 15);
                }
            }
            updateAliens();
            updateBullets();
        } else {
            if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
                resetGame();
            } else if (AntBoy.Buttons.wasPressed(ANT_BTN_B)) {
                running = false;
                break;
            }
        }

        render();
        delay(25);
    }

    saveHighScore();
}
