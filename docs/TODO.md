# ANTBOY (2026) — MASTER TODO & EXECUTION ROADMAP
## Actionable Task Tracker Berdasarkan PRD v1.2 (Gold Master)

> **Status Proyek**: Active / Bring-Up Complete / Masuk Fase AntBoy-Core SDK & AntOS  
> **Target Rilis MVP**: Q4 2026  
> **Lead Developer & Hardware Designer**: Anton Prafanto  
> **Repository**: [antonprafanto/antboy](https://github.com/antonprafanto/antboy.git)

---

## 📌 Quick Status Overview
- [x] **Fase 0: Hardware Bring-Up & Validasi Jalur** (100% Selesai)
- [x] **Fase 1: AntBoy-Core SDK & HAL (Hardware Abstraction Layer)** (100% Selesai & Terverifikasi di Hardware)
- [ ] **Fase 2: AntOS Launcher & UI Engine (320x240 Landscape)** (Ready to Start)
- [ ] **Fase 3: Pilar 1 — Retro Gaming & Entertainment**
- [ ] **Fase 4: Pilar 2 — Wireless & Cyber-Tool**
- [ ] **Fase 5: Pilar 3 — IoT & Smart Home Pocket Controller**
- [ ] **Fase 6: Pilar 4 — Hardware Hacker & Lab Companion (Header J4)**
- [ ] **Fase 7: Tooling Komersial, Factory QC & Casing 3D Print**
- [ ] **Fase 8: Perencanaan Desain PCB Revisi V1.1**

---

## 🚀 Rincian Tugas (Sprint Breakdown)

### FASE 0: Validasi Hardware & Sisa Pengujian Board V1
*Fokus: Memastikan 100% komponen fisik berfungsi sebelum abstraksi software.*
- [x] **Display ST7789 Bring-up**: Inisialisasi SPI 320x240, mode landscape asli (`setRotation(3)`), `invertDisplay(true)`, Backlight `IO14` HIGH.
- [x] **Mapping 10 Tombol Fisik**: Uji digital input tombol A (`IO33`), B (`IO32`), SELECT (`IO27`), START (`IO39`), MENU (`IO13`), VOL (`IO0`).
- [x] **Validasi D-Pad Resistor Ladder**: Pembagian tegangan ADC 12-bit terkalibrasi pada `IO35` (UP/DOWN) dan `IO34` (LEFT/RIGHT).
- [x] **Tuning Akustik Buzzer**: Menemukan titik resonansi piezo di `IO26` (1.7 kHz – 3.1 kHz) untuk kompensasi R1 1kΩ.
- [x] **Keamanan Repositori Git**: File privat KiCad (`.kicad_sch`, `.kicad_pcb`) dilindungi, hanya merilis Gerber produksi (`ANTBOY_V4.zip`).
- [ ] **MicroSD Card SPI Mounting Test**: Uji pembacaan FAT32 pada pin CS `IO22` (VSPI berbagi bus dengan ST7789).
- [ ] **Uji Kontinuitas Pin Bebas J4**: Verifikasi pin `IO4`, `IO16`, dan `IO25` pada header samping 15-pin.

---

### FASE 1: AntBoy-Core SDK & HAL (Pondasi Software) — [SELESAI / VERIFIED]
*Fokus: Membuat pustaka `AntBoy.h` agar kodingan game/app bersih, modular, dan mudah digunakan komunitas.*
- [x] **Konfigurasi PlatformIO & Partisi Flash (`partitions.csv`)**:
  - [x] Buat skema partisi kustom 4MB (`partitions.csv`) dengan alokasi app ~1.9 MB untuk dual-OTA.
  - [x] Optimasi compiler build flags di `platformio.ini` (`-O2`, CPU Freq 240MHz, `-DCORE_DEBUG_LEVEL=0`).
- [x] **Struktur Folder SDK**: Direktori resmi `firmware/lib/AntBoy/` dengan `AntBoy.h` dan `AntBoy.cpp`.
- [x] **Modul `AntBoy_Buttons`**:
  - [x] Debouncing internal (filter bising tombol fisik).
  - [x] Fungsi event: `isPressed(btn)`, `wasPressed(btn)`, `wasReleased(btn)`.
  - [x] Pembacaan arah D-Pad via enum: `readDpad()` (`DIR_NONE`, `DIR_UP`, `DIR_DOWN`, `DIR_LEFT`, `DIR_RIGHT`).
  - [x] **System-Wide Shortcuts**: Deteksi kombinasi tombol global tahan `SELECT + START` 2 detik untuk soft reset / exit.
- [x] **Modul `AntBoy_Display` (High-Speed ST7789 Rendering)**:
  - [x] Driver ST7789 landscape 320x240 teroptimasi (`setRotation(3)`, `invertDisplay(true)`).
  - [x] Helper rendering: `fillScreen()`, `drawCenteredText()`, `drawHeaderBar()`, `drawFooterBar()`.
  - [x] Brightness control via PWM timer LEDC pada pin `IO14` (5 kHz).
- [x] **Modul `AntBoy_Audio`**:
  - [x] Driver nada frekuensi: `playTone(freq, dur)`.
  - [x] Preset melodi resonansi: `playStartupJingle()`, `playClick()`, `playConfirm()`, `playWarning()`.
  - [x] Software volume control: `cycleVolume()` (Mute, 25%, 50%, 75%, 100%).
- [x] **Modul `AntBoy_SD` & SPI Mutex**:
  - [x] Inisialisasi MicroSD pada `IO22` dengan FreeRTOS Mutex (`spi_bus_mutex`) untuk isolasi display CS `IO5`.
  - [x] Helper pembaca file cepat dan pembuat direktori standar konsol.
- [x] **Modul `AntBoy_Power`**:
  - [x] Konfigurasi Deep Sleep dengan interrupt wakeup eksternal pada tombol MENU (`IO13`) atau START (`IO39`).
- [x] **Tools Bantu Aset (`tools/`)**:
  - [x] Skrip Python `tools/png_to_rgb565.py` untuk konversi gambar PNG/BMP ke C-array format RGB565.

---

### FASE 2: AntOS Launcher & UI Engine
*Fokus: Antarmuka sistem operasi retro-futuristik dengan navigasi carousel menu.*
- [ ] **AntOS Bootloader Sequence**:
  - [ ] Animasi Splash Screen logo ANTBOY dengan jingle audio boot.
  - [ ] Pengecekan kartu SD otomatis saat booting (mount check).
- [ ] **Carousel Tile Launcher (Menu Utama)**:
  - [ ] Menu geser horizontal menampilkan 4 pilar fitur: Gaming, Wireless, IoT, Lab Companion.
  - [ ] Animasi transisi tile halus pada target 40–60 FPS.
- [ ] **Top Status Bar & OSD**:
  - [ ] Indikator status Wi-Fi / Bluetooth.
  - [ ] OSD volume popup saat tombol `VOL` ditekan.
  - [ ] Indikator status MicroSD terpasang/tidak.
- [ ] **Menu Quick Settings**:
  - [ ] Pengaturan tingkat kecerahan layar (20% – 100%).
  - [ ] Pengaturan volume suara (Mute s.d. Max).
  - [ ] Penyimpanan konfigurasi ke Flash NVS (`settings.json`).

---

### FASE 3: Pilar 1 — Retro Gaming & Entertainment
*Fokus: Fitur hiburan konsol retro klasik.*
- [ ] **Native 8-bit Mini Games (Standalone tanpa MicroSD)**:
  - [ ] Game 1: **Snake Retro** (klasik ular dengan high score NVS).
  - [ ] Game 2: **Tetris Pocket** (game susun balok dengan efek suara).
  - [ ] Game 3: **Space Invaders** (arcade tembak-menembak sederhana).
  - [ ] Game 4: **Pong / Breakout**.
- [ ] **Chiptune Audio Player**:
  - [ ] Pemutar lagu 8-bit dari kartu SD format `.mid` / `.rtttl`.
  - [ ] Visualisator gelombang audio real-time di layar ST7789.
- [ ] **Game Boy Emulator Core (Peanut-GB Port)**:
  - [ ] Integrasi engine Peanut-GB ke arsitektur FreeRTOS Core 1.
  - [ ] Browser file pemilihan ROM `.gb` dan `.gbc` dari folder `/roms/gb/`.
  - [ ] Framebuffer blit via SPI DMA dengan target performa ~60 FPS (frame-skip opsional).
  - [ ] Mapping kontrol tombol fisik ANTBOY ke tombol Game Boy asli.
  - [ ] Fitur Save & Load State ke file `/roms/saves/*.sav` di MicroSD.

---

### FASE 4: Pilar 2 — Wireless & Cyber-Tool
*Fokus: Alat audit nirkabel saku (Pocket Swiss-Army Knife).*
- [ ] **Wi-Fi Spectrum & Channel Waterfall**:
  - [ ] Pemindaian seluruh 14 kanal Wi-Fi 2.4 GHz.
  - [ ] Grafik waterfall dan diagram batang intensitas sinyal (RSSI).
  - [ ] Deteksi kanal terpadat untuk optimasi router.
- [ ] **Wi-Fi Packet Monitor (Passive Sniffer)**:
  - [ ] Penghitung paket beacon / data di udara (Promiscuous mode).
  - [ ] Deteksi Access Point baru dan pencatatan log ke `/logs/wifi_survey.csv`.
- [ ] **BLE Beacon & Device Scanner**:
  - [ ] Pemindai perangkat Bluetooth (smartwatch, TWS, beacon, AirTag).
  - [ ] Estimasi jarak berbasis kalkulasi log-distance RSSI.
- [ ] **Bluetooth Virtual Gamepad HID**:
  - [ ] Profil BLE Gamepad agar ANTBOY bisa dijadikan controller nirkabel untuk PC, Android, atau Switch.
- [ ] **Wireless Presentation Clicker**:
  - [ ] Profil BLE Keyboard untuk kendali slide (Next: Button A, Prev: Button B, Blackout: START).

---

### FASE 5: Pilar 3 — IoT & Smart Home Pocket Controller
*Fokus: Remote saku untuk otomasi rumah dan komunikasi darurat.*
- [ ] **ESP-NOW Off-Grid Walkie-Talkie**:
  - [ ] Komunikasi teks instan antar ANTBOY tanpa internet/router hingga radius 150m.
  - [ ] Keyboard virtual on-screen yang dikontrol menggunakan D-Pad.
- [ ] **MQTT Smart Home Remote Dashboard**:
  - [ ] Klien MQTT ringan via Wi-Fi rumah.
  - [ ] Tampilan kartu saklar ON/OFF untuk lampu/perangkat Home Assistant.
- [ ] **Desk Clock & Weather Display**:
  - [ ] Mode jam meja saat ANTBOY sedang di-charge.
  - [ ] Sinkronisasi waktu internet via NTP + suhu lokal via OpenWeatherMap API.

---

### FASE 6: Pilar 4 — Hardware Hacker & Lab Companion (J4 Header)
*Fokus: Asisten lab saku memanfaatkan header samping 15-pin.*
- [ ] **Portable UART Serial Monitor**:
  - [ ] Membaca log serial eksternal melalui pin `IO16` (RX).
  - [ ] Pilihan baud rate (9600, 19200, 38400, 57600, 115200).
  - [ ] Terminal teks layar auto-scroll dengan fitur pause / simpan log ke MicroSD.
- [ ] **I2C Bus Scanner & Auto-Detect**:
  - [ ] I2C Master pada pin bebas `IO4` (SDA) dan `IO16` (SCL).
  - [ ] Pemindaian 127 alamat I2C + deteksi otomatis sensor populer (BME280, MPU6050, OLED, dll).
- [ ] **PWM & Frequency Signal Generator**:
  - [ ] Pembangkit sinyal PWM pada pin `IO25` (DAC) atau `IO4`.
  - [ ] Pengaturan frekuensi (1 Hz – 50 kHz) dan duty cycle (0–100%) dengan tombol D-Pad.
- [ ] **Mini Logic Probe**:
  - [ ] Pembaca status logika (HIGH, LOW, FREQUENCY) pada sirkuit eksternal.

---

### FASE 7: Produksi, QA & Tooling Komersial
*Fokus: Kesiapan manufaktur, kemudahan pengguna awam, dan casing fisik.*
- [ ] **Firmware Factory QC Jig (`factory_test.bin`)**:
  - [ ] Program self-test otomatis untuk perakitan unit sebelum dikirim ke pembeli.
  - [ ] Uji 5 warna layar (Red, Green, Blue, White, Black) untuk deteksi dead-pixel.
  - [ ] Uji tekan 10 tombol fisik dengan indikator visual hijau dan nilai ADC D-Pad.
  - [ ] Uji baca/tulis berkas 512KB ke MicroSD.
  - [ ] Uji sapuan suara buzzer (1.7 kHz – 3.1 kHz).
  - [ ] Uji penerimaan sinyal Wi-Fi & BLE.
- [ ] **Desain Casing 3D Print (STL/STEP)**:
  - [ ] Desain casing *sandwich* (pelat depan & belakang) memanfaatkan **5 lubang baut M3**.
  - [ ] Ruang ceruk (*cavity clearance*) 1.0 mm di pelat depan untuk komponen SMD tengah.
  - [ ] Desain tombol plunger 3D print untuk tombol 6x6mm.
  - [ ] Kompartemen baterai Li-Po (model 503040 / 603048).
- [ ] **Zero-Install Web Serial Flasher**:
  - [ ] Setup halaman web browser flasher (ESP Web Tools) di GitHub Pages / domain resmi.
- [ ] **Dokumentasi Kit Komersial**:
  - [ ] Panduan perakitan bergambar untuk pembeli Tier 1 (Bare PCB) & Tier 2 (Kit DIY).
  - [ ] Kartu fisik kartu QR menuju `s.id/antonprafanto`.

---

### FASE 8: Perencanaan Desain PCB Hardware V1.1 (Next Revision)
*Fokus: Rencana penyempurnaan hardware masa depan di KiCad.*
- [ ] **Audio Buzzer**: Ganti resistor R1 dari 1kΩ ke 33–100Ω atau pasang transistor NPN S8050 untuk volume suara maksimal.
- [ ] **Battery Fuel Gauge**: Tambahkan pembagi tegangan 100kΩ / 100kΩ dari jalur baterai ke pin ADC `IO36` (VP/A0).
- [ ] **Power Header J4**: Tambahkan pin daya VCC (+3.3V, +5V) dan GND pada header ekspansi modular.
- [ ] **Battery Connector**: Tambahkan footprint konektor baterai JST-PH 2.0mm onboard.

---

*TODO Document ini disinkronkan secara berkala dengan progress pengerjaan proyek ANTBOY.*
