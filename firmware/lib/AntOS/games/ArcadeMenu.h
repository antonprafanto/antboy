#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"
#include "SnakeGame.h"
#include "TetrisGame.h"
#include "SpaceInvadersGame.h"
#include "BreakoutGame.h"

class ArcadeMenuClass {
public:
    void run();

private:
    uint8_t selectedIndex = 0;
    void render();
};

extern ArcadeMenuClass ArcadeMenu;
