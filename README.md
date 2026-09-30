# ANTBOY (2026) Multi-Purpose Cyber-Gaming Handheld

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Arduino%20ESP32-orange.svg)](https://platformio.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/badge/Hardware-V1.0%20Bring--Up%20Passed-brightgreen.svg)]()
[![Hardware](https://img.shields.io/badge/MCU-ESP32%20240MHz%20Dual--Core-blue.svg)]()

**ANTBOY (2026)** adalah perangkat saku serbaguna (*multi-purpose handheld device*) berbasis mikrokontroler **ESP32**, didesain oleh **Anton Prafanto** menggunakan **KiCad 10**. ANTBOY memadukan ergonomi konsol game retro legendaris (Game Boy) dengan fungsionalitas mutakhir untuk *cyber-security tooling*, *IoT smart controller*, dan *hardware hacker lab companion*.

---

## 🌟 4 Pilar Fungsionalitas ANTBOY

1. **Pilar 1: Retro Gaming & Entertainment**: Port Peanut-GB (Game Boy / GBC emulator), game 8-bit native (Snake, Tetris, Space Invaders), dan Chiptune audio player.
2. **Pilar 2: Wireless & Cyber-Tool**: Wi-Fi network scanner, packet monitor, BLE beacon hunter, dan BLE wireless gamepad controller.
3. **Pilar 3: IoT & Smart Home Pocket Controller**: ESP-NOW low-latency peer chat, MQTT client dashboard, Jam & Cuaca online.
4. **Pilar 4: Hardware Hacker & Lab Companion**: Antarmuka ekspansi 15-pin (J4) dengan UART monitor serial, I2C bus scanner, GPIO logic probe, dan DAC signal generator.

---

## 🎮 Spesifikasi Teknis Hardware

| Komponen | Spesifikasi Teknis | Keterangan Rangkaian |
|---|---|---|
| **MCU** | ESP32-WROOM-32 / Wemos D1 Mini32 | Dual-Core Tensilica Xtensa LX6 @ 240MHz, 520KB SRAM, 4MB Flash |
| **Display** | 2.0" IPS TFT LCD (320x240 Landscape) | Driver IC: **ST7789** (SPI 40MHz, Invert True, Backlight 5kHz LEDC PWM) |
| **Audio** | Piezo Buzzer Onboard | GPIO 26 (LEDC Timer PWM Channel 2, Resonansi 1.7–3.1 kHz) |
| **Storage** | MicroSD Card Slot (FAT32/SDHC) | CS: GPIO 22, VSPI Shared Bus dengan FreeRTOS Mutex Arbitration |
| **Status LED** | SMD Green LED D1 | GPIO 2 (Series R10 10kΩ ke GND) |
| **D-Pad** | Dual-Channel Resistor Ladder ADC | UP/DOWN: `GPIO 35`, LEFT/RIGHT: `GPIO 34` (3-sample median filter) |
| **Action Buttons** | Digital Active-LOW Tact Switch | Button A (`GPIO 33`), Button B (`GPIO 32`) |
| **Function Buttons**| Digital Active-LOW Tact Switch | SELECT (`GPIO 27`), START (`GPIO 39`), MENU (`GPIO 13`), VOL (`GPIO 0`) |
| **Ekspansi (J4)** | 15-Pin Side Expansion Header | 3 Pin Bebas Tanpa Konflik: `GPIO 4`, `GPIO 16`, `GPIO 25` (DAC1) |
| **Mekanikal** | 68.50 mm × 84.00 mm | 5x Lubang Baut Standar M3 (Drill 3.0 mm, Pad 4.0 mm) |

---

## 🛠️ Pinout Lengkap & Matriks Ekspansi (J4)

```text
                       HEADER EKSPANSI 15-PIN (J4)
+------+--------+---------------------+------------------------------------------+
| Pin  | Net    | Status Internal     | Rekomendasi Penggunaan Add-on            |
+------+--------+---------------------+------------------------------------------+
| 1    | IO4    | BEBAS (Isolasi)     | GPIO Bebas / I2C SDA / Touch 0 / ADC2    |
| 2    | IO5    | SHARED: ST7789 CS   | JANGAN DIGUNAKAN saat LCD aktif          |
| 3    | IO13   | SHARED: Tombol MENU | Input Interrupt / Active-LOW Button      |
| 4    | IO14   | SHARED: Backlight   | JANGAN DIGUNAKAN (Khusus kontrol layar)  |
| 5    | IO16   | BEBAS (Isolasi)     | UART2 RX2 / I2C SCL / 1-Wire / NeoPixel  |
| 6    | IO18   | SHARED: SPI SCK     | SPI Bus Clock untuk Add-on Shield        |
| 7    | IO19   | SHARED: SD MISO     | SPI Bus MISO untuk Add-on Shield         |
| 8    | IO21   | SHARED: ST7789 DC   | JANGAN DIGUNAKAN saat render grafis      |
| 9    | IO22   | SHARED: SD Card CS  | JANGAN DIGUNAKAN untuk I2C (Gunakan CS)  |
| 10   | IO23   | SHARED: SPI MOSI    | SPI Bus MOSI untuk Add-on Shield         |
| 11   | IO25   | BEBAS (Isolasi)     | DAC Channel 1 / Analog Out / Servo PWM   |
| 12   | IO26   | SHARED: Buzzer J3   | Output Audio / PFM Tone                  |
| 13   | IO27   | SHARED: Tombol SEL  | Input Interrupt Eksternal                |
| 14   | IO32   | SHARED: Tombol B    | Input Button Eksternal                   |
| 15   | IO33   | SHARED: Tombol A    | Input Button Eksternal                   |
+------+--------+---------------------+------------------------------------------+
```

---

## 📂 Struktur Repositori

```text
├── ANTBOY/                    # File produksi pabrikasi Gerber & Drill
├── ANTBOY_V4.zip              # Arsip rilis Gerber V4 siap cetak (JLCPCB / PCBWay)
├── docs/                      # Dokumentasi teknis & perancangan
│   ├── PRD_ANTBOY_2026.md     # Product Requirement Document (PRD Gold Master v1.2)
│   └── TODO.md                # Master Roadmap & Milestone Tracker
├── tools/                     # Skrip utilitas & otomasi aset
│   └── png_to_rgb565.py       # Konverter gambar PNG/BMP ke C-Array RGB565 PROGMEM
└── firmware/                  # Proyek firmware PlatformIO (Arduino Framework)
    ├── partitions.csv         # Skema partisi flash 4MB kustom (Dual-OTA ~1.9MB)
    ├── platformio.ini         # Konfigurasi board, 240MHz CPU, 80MHz QIO, -O2 flags
    ├── lib/AntBoy/            # Pustaka Resmi AntBoy-Core SDK & HAL
    │   ├── AntBoy.h / .cpp    # Master Singleton AntBoy Class
    │   ├── AntBoy_Pins.h      # Definisi terpusat seluruh pinout board V1
    │   ├── AntBoy_Buttons.*   # Debounce 18ms, ADC median filter, combo shortcut
    │   ├── AntBoy_Display.*   # ST7789 320x240 landscape, 5kHz LEDC PWM backlight
    │   ├── AntBoy_Audio.*     # LEDC duty-cycle volume attenuation, resonansi 1.7-3.1kHz
    │   ├── AntBoy_SD.*        # FreeRTOS SPI bus mutex, CS isolation, thread-safe I/O
    │   └── AntBoy_Power.*     # Deep sleep power management & RTC wake-up
    └── src/
        └── main.cpp           # Interactive diagnostic visualizer & bring-up firmware
```

---

## 💻 Contoh Penggunaan AntBoy-Core SDK

```cpp
#include <Arduino.h>
#include <AntBoy.h>

void setup() {
    // Inisialisasi seluruh periferal ANTBOY (Display, Audio, Buttons, SD)
    AntBoy.begin(true);

    // Mainkan startup jingle bawaan
    AntBoy.Audio.playStartupJingle();

    // Gambar teks di layar ST7789
    AntBoy.Display.fillScreen(ANTBOY_COLOR_BLACK);
    AntBoy.Display.drawHeaderBar("ANTBOY (2026)");
    AntBoy.Display.drawCenteredText("HELLO ANTBOY!", 100, ANTBOY_COLOR_GREEN, 2);
}

void loop() {
    // Wajib dipanggil di setiap awal iterasi loop untuk polling tombol
    AntBoy.update();

    // Cek event tombol
    if (AntBoy.Buttons.wasPressed(ANT_BTN_A)) {
        AntBoy.Audio.playClick();
        AntBoy.toggleLED();
    }

    // Deteksi D-Pad
    AntDirection dir = AntBoy.Buttons.readDpad();
    if (dir == ANT_DIR_UP) {
        AntBoy.Display.cycleBrightness();
    }

    // Deteksi shortcut keluar global (Tahan SELECT + START 2 detik)
    if (AntBoy.checkExitShortcut()) {
        Serial.println("Exiting to Launcher...");
    }

    delay(16); // ~60 FPS
}
```

---

## 🚀 Kompilasi & Upload Firmware (PlatformIO)

1. Pastikan Anda memiliki [PlatformIO CLI](https://platformio.org/) atau VS Code PlatformIO extension.
2. Hubungkan modul Wemos D1 Mini32 ANTBOY ke port USB komputer.
3. Masuk ke direktori `firmware/` dan jalankan upload:
   ```bash
   pio run -t upload
   ```
4. Buka Serial Monitor (baud rate `115200`):
   ```bash
   pio device monitor -b 115200
   ```

---

## 📄 Lisensi & Hak Cipta
Dirancang dan dikembangkan dengan bangga oleh **Anton Prafanto** (2026).  
Tautan proyek: [s.id/antonprafanto](https://s.id/antonprafanto) | [GitHub: antonprafanto/antboy](https://github.com/antonprafanto/antboy.git)
