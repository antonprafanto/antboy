#include "AntBoy_SD.h"

bool AntBoy_SDClass::begin() {
    if (_spiMutex == NULL) {
        _spiMutex = xSemaphoreCreateMutex();
    }

    if (!lockBus(200)) {
        return false;
    }

    // Pastikan kedua CS berstatus HIGH (Deselected) sebelum inisialisasi
    pinMode(ANTBOY_PIN_SD_CS, OUTPUT);
    digitalWrite(ANTBOY_PIN_SD_CS, HIGH);
    pinMode(ANTBOY_PIN_TFT_CS, OUTPUT);
    digitalWrite(ANTBOY_PIN_TFT_CS, HIGH);

    // Inisialisasi kartu SD pada frekuensi aman 20 MHz
    _isMounted = SD.begin(ANTBOY_PIN_SD_CS, SPI, 20000000);

    unlockBus();
    return _isMounted;
}

void AntBoy_SDClass::end() {
    if (_isMounted) {
        SD.end();
        _isMounted = false;
    }
}

uint64_t AntBoy_SDClass::totalBytes() const {
    if (!_isMounted) return 0;
    return SD.totalBytes();
}

uint64_t AntBoy_SDClass::usedBytes() const {
    if (!_isMounted) return 0;
    return SD.usedBytes();
}

const char* AntBoy_SDClass::cardTypeString() const {
    if (!_isMounted) return "None";
    uint8_t cardType = SD.cardType();
    switch (cardType) {
        case CARD_MMC:  return "MMC";
        case CARD_SD:   return "SDSC";
        case CARD_SDHC: return "SDHC";
        default:        return "Unknown";
    }
}

bool AntBoy_SDClass::createStandardDirectories() {
    if (!_isMounted) return false;

    if (!lockBus(100)) return false;

    const char* dirs[] = {
        "/antos",
        "/roms",
        "/roms/gb",
        "/roms/nes",
        "/roms/sms",
        "/roms/atari",
        "/roms/saves",
        "/apps",
        "/music",
        "/logs",
        "/screenshots"
    };

    for (const char* dir : dirs) {
        if (!SD.exists(dir)) {
            SD.mkdir(dir);
        }
    }

    unlockBus();
    return true;
}

bool AntBoy_SDClass::exists(const char* path) {
    if (!_isMounted) return false;
    if (!lockBus(100)) return false;
    bool res = SD.exists(path);
    unlockBus();
    return res;
}

String AntBoy_SDClass::readFile(const char* path) {
    if (!_isMounted) return String();
    if (!lockBus(200)) return String();
    File f = SD.open(path, FILE_READ);
    if (!f) {
        unlockBus();
        return String();
    }
    String content = f.readString();
    f.close();
    unlockBus();
    return content;
}

bool AntBoy_SDClass::writeFile(const char* path, const char* message) {
    if (!_isMounted) return false;
    if (!lockBus(200)) return false;
    File f = SD.open(path, FILE_WRITE);
    if (!f) {
        unlockBus();
        return false;
    }
    size_t written = f.print(message);
    f.close();
    unlockBus();
    return written > 0;
}

bool AntBoy_SDClass::appendFile(const char* path, const char* message) {
    if (!_isMounted) return false;
    if (!lockBus(200)) return false;
    File f = SD.open(path, FILE_APPEND);
    if (!f) {
        unlockBus();
        return false;
    }
    size_t written = f.println(message);
    f.close();
    unlockBus();
    return written > 0;
}

bool AntBoy_SDClass::lockBus(uint32_t timeoutMs) {
    if (_spiMutex == NULL) return true;
    if (xSemaphoreTake(_spiMutex, pdMS_TO_TICKS(timeoutMs)) == pdTRUE) {
        // Isolasi Layar: Paksa Display CS HIGH saat MicroSD diakses
        digitalWrite(ANTBOY_PIN_TFT_CS, HIGH);
        return true;
    }
    return false;
}

void AntBoy_SDClass::unlockBus() {
    if (_spiMutex != NULL) {
        // Kembalikan SD CS ke HIGH setelah transaksi selesai
        digitalWrite(ANTBOY_PIN_SD_CS, HIGH);
        xSemaphoreGive(_spiMutex);
    }
}
