#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"

class SpaceInvadersGameClass {
public:
    void run();

private:
    static const int ALIEN_ROWS = 4;
    static const int ALIEN_COLS = 6;
    static const int MAX_BOMBS = 4;

    struct Alien {
        int x, y;
        bool alive;
        uint8_t type;
    };

    struct Bullet {
        int x, y;
        bool active;
    };

    Alien aliens[ALIEN_ROWS][ALIEN_COLS];
    int alienDir;
    int alienStepTimer;
    int alienAnimFrame;
    int livingAliens;

    int playerX;
    int playerLives;
    Bullet playerBullet;

    Bullet alienBombs[MAX_BOMBS];

    int ufoX;
    bool ufoActive;
    uint32_t lastUfoSpawn;

    int score;
    int prevScore;
    int highScore;
    int wave;
    int prevWave;
    int prevLives;
    bool gameOver;
    bool needsFullRedraw;

    void resetGame();
    void initWave();
    void updateAliens();
    void updateBullets();
    void spawnUfo();
    void render();
    void loadHighScore();
    void saveHighScore();
};

extern SpaceInvadersGameClass SpaceInvadersGame;
