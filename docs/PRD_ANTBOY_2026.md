# PRODUCT REQUIREMENT DOCUMENT (PRD)
## ANTBOY (2026) — Multi-Purpose Cyber-Gaming Handheld Device

---

| Dokumen | Nilai |
|---|---|
| **Nama Proyek** | **ANTBOY (2026) Multi-Purpose Device** |
| **Versi Dokumen** | 1.0 (Draft Arsitektur & Perencanaan Rilis) |
| **Status** | Active / In Development |
| **Lead Developer & Hardware Designer** | Anton Prafanto |
| **Target Rilis MVP** | Q4 2026 |
| **Lisensi Hardware** | Proprietary (Gerber-only distribution) |
| **Lisensi Software** | Open-Core / MIT (Firmware & Community Apps) |

---

## 1. Executive Summary & Visi Produk

### 1.1 Latar Belakang & Masalah Pasar
Pasar konsol genggam retro DIY saat ini terpecah menjadi dua kategori ekstrem:
1. **Konsol Game Murni**: Hanya berfungsi untuk bermain game retro (misal: Arduboy, Odroid-Go), tanpa kemampuan konektivitas atau kegunaan di dunia kerja nyata.
2. **Gadget Hacker/Hacker Tool**: Berfungsi untuk network/hardware security (misal: Flipper Zero, M5Stack Cardputer), namun tidak memiliki ergonomi controller game yang nyaman untuk hiburan harian.

### 1.2 Visi & Nilai Unik ANTBOY
**ANTBOY (2026)** menjembatani kedua dunia tersebut menjadi sebuah perangkat **EDC (Everyday Carry)** saku multifungsi:
> *"Bermain game retro klasik di saat santai, lalu beralih menjadi alat audit jaringan nirkabel, kontroler smart home, dan asisten lab elektronika saat bekerja."*

Perangkat ini dirancang dengan PCB 2-layer kustom, ditenagai mikrokontroler **ESP32 Dual-Core (Wi-Fi + BLE)**, layar tajam **2.0" IPS 320x240**, tata letak tombol ergonomis Game Boy, slot MicroSD, dan **Expansion Header GPIO** di sisi samping untuk modul modular (*backpack/cartridge*).

---

## 2. Target Pengguna & Persona

1. **Maker & Hardware Hacker**: Membutuhkan alat saku untuk membaca sinyal serial (UART), memindai sensor I2C, dan bereksperimen dengan protokol radio nirkabel tanpa harus membawa laptop.
2. **Mahasiswa / Pelajar Teknik Elektro & IT**: Menginginkan platform belajar mikrokontroler, FreeRTOS, dan pengembangan game embedded yang terjangkau dan menyenangkan.
3. **Retro Gaming Enthusiast**: Penggemar konsol portabel yang ingin memainkan game 8-bit klasik dan game homebrew buatan komunitas.
4. **Sysadmin & IoT Engineer**: Profesional yang memerlukan alat praktis untuk mengecek kanal Wi-Fi, menguji sinyal BLE, atau mengontrol perangkat MQTT smart home di lapangan.

---

## 3. Spesifikasi Hardware (Hardware Baseline V1)

```
                 +------------------------------------------+
                 |       ANTBOY (2026) Hardware Core        |
                 +------------------------------------------+
                                      |
         +----------------------------+----------------------------+
         |                            |                            |
  [ Input Subsystem ]        [ Compute Subsystem ]       [ Output Subsystem ]
  - D-Pad (IO34, IO35)       - ESP32-WROOM-32 (240MHz)   - ST7789 IPS 320x240 (SPI)
  - Action: A/B (IO33/IO32)  - 520 KB SRAM / 4MB Flash   - Piezo Buzzer (IO26)
  - Func: SEL/STA/MEN/VOL    - Wi-Fi 802.11 b/g/n        - Status LED (IO2)
  - Side Expansion GPIO      - Bluetooth 4.2 BR/EDR+BLE  - MicroSD Storage (SPI)
```

| Komponen | Spesifikasi Teknis | Antarmuka / Pinout | Catatan Implementasi |
|---|---|---|---|
| **MCU** | ESP32-D0WD (Wemos D1 Mini32) | 240 MHz Dual-Core, Wi-Fi, BLE | Core 0 untuk OS/Konektivitas, Core 1 untuk UI/Input |
| **Layar Utama** | 2.0" IPS TFT LCD (GMT020-03-SD) | SPI (MOSI:23, SCK:18, CS:5, DC:21, BLK:14) | Mode Landscape: 320x240 px, Sudut Pandang 178° |
| **Storage** | MicroSD Card Slot | SPI Bus (CS dedicated, MOSI:23, SCK:18, MISO:19) | Mendukung FAT32 hingga 32GB (ROM, Logs, Payload) |
| **D-Pad** | 4-Arah (Up, Down, Left, Right) | Dual-Channel Analog Resistor Ladder (IO35, IO34) | 12-Bit ADC, hemat pin ESP32 |
| **Tombol Action** | Tactile Switch Button A & B | Digital Active-LOW (A: IO33, B: IO32) | Internal Pull-Up aktif |
| **Tombol Fungsi** | START, SELECT, MENU, VOL | Digital Active-LOW (STA:39, SEL:27, MEN:13, VOL:0) | START & VOL menggunakan pull-up eksternal 10k |
| **Audio** | Piezoelectric Transducer Buzzer | PWM Frequency Modulation (GPIO 26) | Optimal pada rentang resonansi 1.7 kHz – 3.1 kHz |
| **Ekspansi GPIO** | 15-Pin Side Breakout Bus | IO4, IO5, IO13, IO14, IO16, IO18, IO19, IO21, IO22, IO23, IO25, IO26, IO27, IO32, IO33 | Jalur I2C, SPI, UART, DAC/ADC, dan PWM terbuka |
| **Power Input** | USB Type-C & Baterai Li-Po 3.7V | Regulator 3.3V LDO | Sirkuit charger onboard + proteksi tegangan |

---

## 4. Arsitektur Software: "AntOS"

Sistem operasi berbasis **FreeRTOS** dengan pendekatan modular (*applet-based*). Pengguna berinteraksi melalui **AntOS Launcher** dengan antarmuka bergaya retro-futuristik.

```text
+-------------------------------------------------------------------+
|                           AntOS LAUNCHER                          |
|             (Carousel Menu, Quick Settings, Status Bar)           |
+-------------------+-------------------+-------------------+-------+
|  PILAR 1: GAMING  | PILAR 2: WIRELESS |  PILAR 3: IoT     | PILAR 4: LAB   |
| - Game Boy Emu    | - Wi-Fi Analyzer  | - ESP-NOW Chat    | - UART Monitor |
| - Native Games    | - BLE Scanner     | - MQTT Dashboard  | - I2C Scanner  |
| - Chiptune Audio  | - BLE Gamepad HID | - Clock & Weather | - Logic Probe  |
+-------------------+-------------------+-------------------+----------------+
|                        AntOS CORE SERVICES                         |
|  Power Manager | File System (FAT/LittleFS) | Audio Driver | Graphics API  |
+--------------------------------------------------------------------+
|                         FreeRTOS KERNEL                            |
|             Core 0: Network & I/O  |  Core 1: UI & Engine          |
+--------------------------------------------------------------------+
```

### 4.1 Task Scheduling Dual-Core
- **Core 0 (Background & Network Task)**:
  - Stack Wi-Fi & Bluetooth Stack.
  - Pembacaan streaming berkas dari MicroSD.
  - Background logging & telemetri sensor eksternal.
- **Core 1 (Renderer & Realtime Task)**:
  - Loop grafis rendering layar ST7789 pada target 40–60 FPS.
  - Pembacaan matriks tombol & ADC D-Pad dengan latensi < 10 ms.
  - Driver sintesis nada audio buzzer.

---

## 5. Rincian Kebutuhan Fungsional (4 Pilar Fitur)

### PILAR 1: Retro Gaming & Entertainment Engine

| ID | Fitur | Deskripsi | Kebutuhan Teknis | Prioritas |
|---|---|---|---|---|
| **GAME-01** | **Game Boy Emulator (GB/GBC)** | Menjalankan ROM Game Boy asli dari MicroSD. | Porting Peanut-GB / GNUBoy, DMA SPI transfer, audio mapping ke buzzer/DAC. | 🔴 High |
| **GAME-02** | **Native Mini-Games** | Game bawaan tanpa perlu kartu SD: Snake, Space Invaders, Tetris, Pong. | Engine sprite 2D ringan, state-machine terisolasi, persistent high score di NVS Flash. | 🔴 High |
| **GAME-03** | **Chiptune Tracker Player** | Pemutar musik chiptune 8-bit retro (file `.mid` / `.mod`). | Parser sequence nada, visualisator gelombang audio di layar secara real-time. | 🟡 Medium |
| **GAME-04** | **Save State & Screenshot** | Menyimpan progres game ke kartu MicroSD. | Dump memory RAM emulator ke file `.sav` di MicroSD. | 🟡 Medium |

---

### PILAR 2: Wireless & Cyber-Tool (The Pocket Swiss-Army Knife)

| ID | Fitur | Deskripsi | Kebutuhan Teknis | Prioritas |
|---|---|---|---|---|
| **WIFI-01** | **Wi-Fi Spectrum & RSSI Analyzer** | Memindai SSID 2.4GHz sekitar, mengukur kuat sinyal dan kanal terpadat dalam grafik waterfall. | Wi-Fi Promiscuous mode, visualisasi grafik bar 320x240, deteksi enkripsi (WPA2/WPA3). | 🔴 High |
| **WIFI-02** | **Rogue AP & Packet Monitor** | Mendeteksi keberadaan access point asing, probe request, dan trafik jaringan lokal. | Packet sniffer non-intrusif, logging MAC address ke MicroSD. | 🟡 Medium |
| **BLE-01** | **BLE Device & Beacon Scanner** | Mendeteksi perangkat Bluetooth di sekitar (TWS, AirTag, smartwatch, sensor BLE). | ESP32 BLE GAP Scanner, kalkulasi jarak via RSSI. | 🔴 High |
| **HID-01** | **Bluetooth Virtual Gamepad** | ANTBOY bertindak sebagai controller nirkabel Bluetooth untuk PC, Android, atau Switch. | BLE HID Gamepad profile, pemetaan tombol fisik ANTBOY ke standard gamepad buttons. | 🟠 High |
| **HID-02** | **Wireless Presentation Clicker** | Tombol A/B dan D-Pad digunakan untuk memindah slide presentasi (Next/Prev/Blackout). | BLE Keyboard profile (tombol PgUp, PgDn, F5, Esc). | 🟢 Low |

---

### PILAR 3: IoT & Smart Home Pocket Controller

| ID | Fitur | Deskripsi | Kebutuhan Teknis | Prioritas |
|---|---|---|---|---|
| **IOT-01** | **ESP-NOW Off-Grid Walkie-Talkie** | Komunikasi teks instan antar ANTBOY tanpa internet/router hingga radius 150 meter. | ESP-NOW Broadcast/Unicast protocol, keyboard virtual on-screen di layar ANTBOY. | 🟠 High |
| **IOT-02** | **Smart Home MQTT Dashboard** | Sakelar remote untuk menyalakan lampu, kipas, atau memantau sensor Home Assistant. | Klien MQTT ringan via Wi-Fi rumah, render kartu tombol status ON/OFF. | 🟡 Medium |
| **IOT-03** | **Desk Clock & Weather Display** | Jam meja digital dengan sinkronisasi waktu NTP internet dan prakiraan cuaca otomatis saat charging. | NTP Client, OpenWeatherMap API fetcher, auto-dimming layar saat idle. | 🟢 Low |

---

### PILAR 4: Hardware Hacker & Lab Companion (Pemanfaatan Side GPIO)

| ID | Fitur | Deskripsi | Kebutuhan Teknis | Prioritas |
|---|---|---|---|---|
| **LAB-01** | **Portable UART Serial Monitor** | Menampilkan log serial `Serial.print()` dari board lain langsung ke layar ANTBOY. | Pin `IO16` (RX) & `IO17`/`IO4` (TX), baud rate selector (9600 s.d. 115200), auto-scroll terminal view. | 🔴 High |
| **LAB-02** | **I2C Bus Scanner & Visualizer** | Memindai alamat sensor I2C yang dicolokkan ke header dan membaca datanya secara otomatis. | I2C Master pada `IO22` (SCL) & `IO21`/`IO4` (SDA), database auto-detect driver (BME280, MPU6050, OLED, dll). | 🟠 High |
| **LAB-03** | **PWM & Frequency Signal Generator** | Menghasilkan sinyal PWM untuk pengujian motor servo atau dimmer LED. | ESP32 LEDC PWM generator pada pin `IO25`/`IO4`, slider frekuensi & duty cycle via D-Pad. | 🟡 Medium |
| **LAB-04** | **Mini Logic Probe** | Membaca status logika (HIGH, LOW, PULSE) pada rangkaian eksternal. | Digital input polling / interrupt counter, visualisasi timeline gelombang pulsa. | 🟡 Medium |

---

## 6. Spesifikasi Ekosistem Modul Tambahan (*Add-on Shields*)

Header samping 15-pin ANTBOY dirancang untuk mendukung kartu ekspansi *plug-and-play* yang dapat dijual sebagai aksesori terpisah:

```text
[ ANTBOY Console ] <====== 15-Pin Header ======> [ Add-On Shield / Cartridge ]
                                                   ├── Shield 1: RF Cyber Pack (CC1101 + IR)
                                                   ├── Shield 2: LoRa Long-Range Pack (SX1262)
                                                   ├── Shield 3: Sensor Science Pack (BME280 + MPU6050)
                                                   └── Shield 4: Audio DAC & Headphone Jack Pack
```

1. **RF Cyber Pack (Shield A)**:
   - Chipset: Texas Instruments CC1101 (Sub-1GHz) + IR LED Transmitter & IR Receiver TSOP4838.
   - Fungsi: Analisis remote 433MHz + Remote universal inframerah.
2. **LoRa Long-Range Pack (Shield B)**:
   - Chipset: Semtech SX1262 / SX1276 (915MHz / 868MHz / 433MHz).
   - Fungsi: Komunikasi off-grid darurat hingga puluhan kilometer.
3. **Sensor Science Pack (Shield C)**:
   - Sensor: BME280 (Suhu, Kelembaban, Tekanan Udara) + MPU6050 (Akselerometer 6-Axis untuk motion gaming).

---

## 7. Kebutuhan Non-Fungsional (NFR)

1. **Efisiensi Daya & Baterai**:
   - Daya saat aktif bermain game: **~120–160 mA** (Baterai 1000 mAh bertahan ~6 jam).
   - Daya saat Wi-Fi aktif: **~180–220 mA** (Bertahan ~4.5 jam).
   - Mode **Deep Sleep**: **< 20 µA** (Dapat bertahan berbulan-bulan saat disimpan tanpa baterai drop).
2. **Performa Tampilan**:
   - Refresh rate layar SPI dipertahankan **≥ 40 FPS** menggunakan transfer SPI 40 MHz dengan buffer baris ganda (*double-buffered line rendering*).
3. **Integritas Firmware & Crash Safety**:
   - Proteksi watchdog timer (WDT) untuk mencegah freeze saat koneksi Wi-Fi putus.
   - Safe-mode boot: Tahan tombol `SELECT + START` saat booting untuk masuk ke menu darurat jika aplikasi crash.

---

## 8. Roadmap Pengembangan & Rilis

```mermaid
gantt
    title Roadmap Pengembangan ANTBOY (2026)
    dateFormat  YYYY-MM-DD
    section Fase 1: Core OS
    Hardware Diagnostic & Stabilisasi       :done,    des1, 2026-09-30, 2026-10-07
    AntOS Launcher & Tile GUI Engine        :active,  des2, 2026-10-08, 2026-10-25
    MicroSD Storage & FAT32 Integration    :         des3, 2026-10-26, 2026-11-05
    section Fase 2: Gaming & Fun
    Native 8-bit Mini Games (Snake/Tetris)  :         des4, 2026-11-06, 2026-11-20
    Game Boy Emulator (Peanut-GB Port)      :         des5, 2026-11-21, 2026-12-15
    section Fase 3: Tool & Lab
    Wi-Fi & BLE Analyzer Modules           :         des6, 2026-12-16, 2027-01-10
    UART Terminal & I2C Lab Scanner         :         des7, 2027-01-11, 2027-01-31
    section Fase 4: Komersialisasi
    Desain Casing 3D Print (STL/STEP)       :         des8, 2027-02-01, 2027-02-20
    Dokumentasi Kit DIY & Rilis Penjualan   :         des9, 2027-02-21, 2027-03-15
```

---

## 9. Model Komersialisasi Produk

| Paket Penjualan | Target Pasar | Isi Paket |
|---|---|---|
| **Tier 1: Bare PCB Only** | DIY Maker & Solder Enthusiast | Papan PCB ANTBOY polos + file panduan perakitan & daftar part BOM. |
| **Tier 2: Complete DIY Kit** | Mahasiswa / Hobbyist yang ingin belajar solder | Papan PCB + Modul ESP32 + LCD ST7789 + seluruh komponen SMD/PTH siap rangkai. |
| **Tier 3: Ready-to-Play Edition** | Kolektor & Pengguna Langsung | Perangkat sudah dirakit & diuji penuh, terpasang casing 3D print akrilik/PLA, dan MicroSD terisi game/tools. |
| **Tier 4: Modular Add-ons** | Pengguna yang ingin upgrade fungsi | Shield ekspansi terpisah (RF Cyber Pack, Sensor Pack, LoRa Pack). |

---

*Dokumen ini merupakan panduan arsitektur resmi untuk pengembangan ekosistem ANTBOY (2026).*
