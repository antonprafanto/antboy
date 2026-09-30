#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "../AntOS_Theme.h"

struct ChiptuneTrack {
    const char* title;
    const char* author;
    const char* rtttl;
};

class ChiptunePlayerClass {
public:
    void run();

private:
    int currentTrackIndex = 0;
    bool isPlaying = false;
    bool isPaused = false;

    // Visualizer state
    uint8_t specBands[16];
    uint8_t specPeaks[16];

    void renderUI();
    void updateVisualizer(uint16_t freq);
    void playCurrentTrack();
};

extern ChiptunePlayerClass ChiptunePlayer;
