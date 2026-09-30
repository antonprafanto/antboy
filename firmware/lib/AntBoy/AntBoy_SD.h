#pragma once
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include "AntBoy_Pins.h"

class AntBoy_SDClass {
public:
    bool begin();
    void end();

    bool isMounted() const { return _isMounted; }
    uint64_t totalBytes() const;
    uint64_t usedBytes() const;
    const char* cardTypeString() const;

    // Helper untuk membuat struktur folder standar ANTBOY
    bool createStandardDirectories();

    // Mutex Arbitrasi Bus SPI (Berbagi jalur dengan Layar ST7789)
    bool lockBus(uint32_t timeoutMs = 100);
    void unlockBus();

private:
    bool _isMounted = false;
    SemaphoreHandle_t _spiMutex = NULL;
};
