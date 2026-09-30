#include "AntBoy_Power.h"
#include <esp_sleep.h>

void AntBoy_PowerClass::begin() {
    // Inisialisasi pin status LED D1
    pinMode(ANTBOY_PIN_LED, OUTPUT);
    digitalWrite(ANTBOY_PIN_LED, LOW);
}

void AntBoy_PowerClass::sleep(AntButton wakeButton) {
    // 1. Matikan seluruh periferal konsumsi daya
    analogWrite(ANTBOY_PIN_TFT_BLK, 0); // Matikan lampu layar
    digitalWrite(ANTBOY_PIN_LED, LOW);   // Matikan status LED
    noTone(ANTBOY_PIN_BUZZER);           // Matikan audio buzzer

    // 2. Konfigurasi pin wakeup
    gpio_num_t wakePin = (gpio_num_t)ANTBOY_PIN_BTN_MENU;
    if (wakeButton == ANT_BTN_START) {
        wakePin = (gpio_num_t)ANTBOY_PIN_BTN_START;
    }

    // Tombol aktif LOW (0) saat ditekan
    esp_sleep_enable_ext0_wakeup(wakePin, 0);

    // 3. Masuk ke mode tidur lelap
    esp_deep_sleep_start();
}

bool AntBoy_PowerClass::wasWakeupFromButton() const {
    return (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0);
}
