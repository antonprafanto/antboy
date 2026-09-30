# ANTBOY (2026) Multi-Purpose Device

**ANTBOY** adalah konsol game retro genggam (*handheld retro gaming console*) portabel berbasis mikrokontroler **ESP32**, didesain menggunakan **KiCad 10** dengan layout tombol khas Game Boy dan layar IPS 2.0".

---

## 🎮 Spesifikasi Hardware

| Komponen | Spesifikasi | Keterangan |
|---|---|---|
| **MCU** | ESP32-WROOM-32 / Wemos D1 Mini32 | Dual-Core 240MHz, Wi-Fi & Bluetooth |
| **Display** | 2.0" IPS TFT LCD (240x320) | Driver IC: **ST7789** (SPI Interface) |
| **Audio** | Piezo Buzzer / Speaker | GPIO 26 |
| **Status LED** | LED Indikator | GPIO 2 |
| **Storage** | MicroSD Card Slot | SPI Interface |
| **D-Pad** | Analog Resistor Ladder | UP/DOWN: `GPIO 35`, LEFT/RIGHT: `GPIO 34` |
| **Tombol Action** | Digital (Active-LOW) | Button A (`IO33`), Button B (`IO32`) |
| **Tombol Fungsi** | Digital (Active-LOW) | SELECT (`IO27`), START (`IO39`), MENU (`IO13`), VOL (`IO0`) |

---

## 📂 Struktur Repositori

```text
├── ANTBOY/             # File produksi pabrikasi (Gerber, Drill, Job file)
├── ANTBOY_V4.zip       # Arsip rilis Gerber V4 siap cetak (JLCPCB / PCBWay)
└── firmware/           # Proyek firmware PlatformIO (Arduino Framework)
    ├── src/main.cpp    # Program diagnostik & uji hardware (Layar, Tombol, Buzzer)
    └── platformio.ini  # Konfigurasi board Wemos D1 Mini32 & dependencies
```

---

## 🛠️ Pinout Hardware Detail

### Display ST7789 (SPI)
- **MOSI / SDA**: `GPIO 23`
- **SCLK / SCL**: `GPIO 18`
- **CS**: `GPIO 5`
- **DC**: `GPIO 21`
- **RST**: Terhubung ke Hardware Reset ESP32
- **Backlight (BLK)**: `GPIO 14` (Set HIGH untuk menyalakan lampu latar)

### Kontrol Tombol & Navigasi
- **A**: `GPIO 33` (Internal Pull-Up)
- **B**: `GPIO 32` (Internal Pull-Up)
- **SELECT**: `GPIO 27` (Internal Pull-Up)
- **START**: `GPIO 39 / VN` (External Pull-Up R9 10k)
- **MENU**: `GPIO 13` (Internal Pull-Up)
- **VOL**: `GPIO 0` (External Pull-Up R3 10k)
- **D-Pad Vertikal (UP/DOWN)**: `GPIO 35` (ADC pembagi tegangan)
- **D-Pad Horisontal (LEFT/RIGHT)**: `GPIO 34` (ADC pembagi tegangan)

---

## 🚀 Menjalankan Firmware Uji (PlatformIO)

1. Buka folder `firmware` menggunakan Visual Studio Code dengan ekstensi **PlatformIO IDE**.
2. Hubungkan board ANTBOY ke komputer menggunakan kabel USB.
3. Jalankan kompilasi dan upload firmware:
   ```bash
   pio run -t upload
   ```
4. Buka Serial Monitor (baud rate: `115200`):
   ```bash
   pio device monitor -b 115200
   ```

---

*Desain PCB dan Hardware oleh Anton Prafanto (2026).*
