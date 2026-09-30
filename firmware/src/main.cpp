#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

// =========================================================================
// PILIHAN DRIVER LCD (PILIH SALAH SATU SESUAI MODUL LCD YANG TERPASANG)
// =========================================================================
#define USE_ILI9341      0  // Standar konsol retro 2.4" / 2.8"
#define USE_ST7789       1  // Aktifkan karena modul terdeteksi IC:ST7789 2.0" 240x320
#define USE_ST7735       0  // Aktifkan jika LCD menggunakan modul ST7735

#if USE_ILI9341
  #include <Adafruit_ILI9341.h>
#elif USE_ST7789 || USE_ST7735
  #include <Adafruit_ST7789.h>
#endif

// =========================================================================
// PINOUT HARDWARE ANTBOY (SESUAI SKEMATIK & PCB KICAD)
// =========================================================================
#define TFT_MOSI         23  // SPI MOSI / SDA
#define TFT_SCLK         18  // SPI SCK / SCL
#define TFT_CS            5  // Chip Select
#define TFT_DC           21  // Data / Command
#define TFT_RST          -1  // Terhubung ke hardware reset ESP32 (-1 = tidak dikontrol GPIO)
#define TFT_BACKLIGHT    14  // Wajib HIGH agar lampu layar menyala!

#define BUZZER_PIN       26  // DAC2 / Buzzer
#define LED_PIN           2  // LED Hijau D1

// Tombol Digital (Active-LOW)
#define BTN_A            33
#define BTN_B            32
#define BTN_SELECT       27
#define BTN_START        39  // VN (Input Only, mengandalkan pullup R9 10k)
#define BTN_MENU         13
#define BTN_VOL           0  // IO0 (Pullup R3 10k)

// Tombol D-Pad Analog (Pembagi Tegangan Resistor)
#define DPAD_VERT_PIN    35  // UP & DOWN
#define DPAD_HORZ_PIN    34  // LEFT & RIGHT

// =========================================================================
// INISIALISASI OBJEK DISPLAY
// =========================================================================
#if USE_ILI9341
  Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
  #define COLOR_BLACK       ILI9341_BLACK
  #define COLOR_WHITE       ILI9341_WHITE
  #define COLOR_RED         ILI9341_RED
  #define COLOR_GREEN       ILI9341_GREEN
  #define COLOR_BLUE        ILI9341_BLUE
  #define COLOR_CYAN        ILI9341_CYAN
  #define COLOR_MAGENTA     ILI9341_MAGENTA
  #define COLOR_YELLOW      ILI9341_YELLOW
  #define COLOR_DARKGREY    ILI9341_DARKGREY
  #define COLOR_NAVY        ILI9341_NAVY
  #define COLOR_DARKGREEN   ILI9341_DARKGREEN
#else
  Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
  #define COLOR_BLACK       ST77XX_BLACK
  #define COLOR_WHITE       ST77XX_WHITE
  #define COLOR_RED         ST77XX_RED
  #define COLOR_GREEN       ST77XX_GREEN
  #define COLOR_BLUE        ST77XX_BLUE
  #define COLOR_CYAN        ST77XX_CYAN
  #define COLOR_MAGENTA     ST77XX_MAGENTA
  #define COLOR_YELLOW      ST77XX_YELLOW
  #define COLOR_DARKGREY    0x7BEF
  #define COLOR_NAVY        0x000F
  #define COLOR_DARKGREEN   0x03E0
#endif

// Resolusi Layar Standar
const int SCREEN_W = 240;
const int SCREEN_H = 320;

// State Karakter / Sprite yang dikontrol
int spriteX = SCREEN_W / 2;
int spriteY = 120;
int prevSpriteX = spriteX;
int prevSpriteY = spriteY;
const int spriteSize = 14;
uint16_t spriteColor = COLOR_YELLOW;

// Fungsi audio buzzer sederhana
void playTone(int freq, int durationMs) {
  #if ESP_IDF_VERSION_MAJOR >= 5
    // ESP32 Arduino Core 3.x
    tone(BUZZER_PIN, freq, durationMs);
  #else
    // Legacy support
    tone(BUZZER_PIN, freq, durationMs);
  #endif
}

void playStartupMelody() {
  playTone(523, 80); delay(100);  // C5
  playTone(659, 80); delay(100);  // E5
  playTone(784, 80); delay(100);  // G5
  playTone(1046, 160); delay(180); // C6 (Coin / Startup sound)
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=================================");
  Serial.println("  ANTBOY (2026) - Hardware Test  ");
  Serial.println("=================================");

  // 1. Hidupkan Lampu Latar (Backlight) & Indikator
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH); // Sangat penting! Layar gelap tanpa ini.
  Serial.println("[OK] Backlight ON (GPIO 14)");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(BUZZER_PIN, OUTPUT);

  // 2. Setup Tombol Digital
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_MENU, INPUT_PULLUP);
  pinMode(BTN_VOL, INPUT_PULLUP);
  pinMode(BTN_START, INPUT); // Pin 39 (VN) tidak memiliki internal pullup

  // 3. Setup ADC D-Pad
  pinMode(DPAD_VERT_PIN, INPUT);
  pinMode(DPAD_HORZ_PIN, INPUT);
  analogReadResolution(12);

  // 4. Inisialisasi Layar
  #if USE_ILI9341
    tft.begin();
    tft.setRotation(0); // 0 = Portrait (240x320)
  #elif USE_ST7789
    tft.init(240, 320);
    tft.setRotation(2); // 2 = Portrait orientasi PCB Antboy
    tft.invertDisplay(true); // Wajib true untuk modul ST7789 IPS agar warna tidak negatif/terbalik
  #elif USE_ST7735
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(0);
  #endif

  tft.fillScreen(COLOR_BLACK);
  Serial.println("[OK] Display Initialized");

  // Render Header UI
  tft.fillRect(0, 0, SCREEN_W, 32, COLOR_NAVY);
  tft.drawFastHLine(0, 32, SCREEN_W, COLOR_CYAN);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 8);
  tft.print("ANTBOY (2026)");

  // Area arena kontrol
  tft.drawRect(8, 42, SCREEN_W - 16, 150, COLOR_DARKGREY);
  tft.setTextColor(COLOR_DARKGREY);
  tft.setTextSize(1);
  tft.setCursor(16, 48);
  tft.print("Sprite Arena (Use D-Pad to move)");

  // Suara Startup
  playStartupMelody();
}

void loop() {
  // =========================================================================
  // 1. PEMBACAAN TOMBOL DIGITAL
  // =========================================================================
  bool pressA      = (digitalRead(BTN_A) == LOW);
  bool pressB      = (digitalRead(BTN_B) == LOW);
  bool pressSelect = (digitalRead(BTN_SELECT) == LOW);
  bool pressStart  = (digitalRead(BTN_START) == LOW);
  bool pressMenu   = (digitalRead(BTN_MENU) == LOW);
  bool pressVol    = (digitalRead(BTN_VOL) == LOW);

  // =========================================================================
  // 2. PEMBACAAN D-PAD ANALOG (ADC 12-BIT: 0 - 4095)
  // =========================================================================
  int adcVert = analogRead(DPAD_VERT_PIN); // IO35: UP / DOWN
  int adcHorz = analogRead(DPAD_HORZ_PIN); // IO34: LEFT / RIGHT

  // Ambang batas tegangan pembagi resistor 10k / 10k:
  // - Nilai default (idle / terbuka)   : < 600
  // - Nilai tegangan tengah (~1.65V)   : 1200 s/d 2800
  // - Nilai tegangan penuh (3.3V)      : > 3100
  bool pressUp    = (adcVert > 3000);
  bool pressDown  = (adcVert >= 1000 && adcVert <= 2900);
  bool pressLeft  = (adcHorz > 3000);
  bool pressRight = (adcHorz >= 1000 && adcHorz <= 2900);

  bool anyInput = pressA || pressB || pressSelect || pressStart || 
                  pressMenu || pressVol || pressUp || pressDown || 
                  pressLeft || pressRight;

  // Indikator LED menyala jika ada tombol aktif
  digitalWrite(LED_PIN, anyInput ? HIGH : LOW);

  // =========================================================================
  // 3. LOGIKA GERAKAN SPRITE
  // =========================================================================
  int moveSpeed = 4;
  prevSpriteX = spriteX;
  prevSpriteY = spriteY;

  if (pressUp && spriteY > 60) {
    spriteY -= moveSpeed;
  }
  if (pressDown && spriteY < 172) {
    spriteY += moveSpeed;
  }
  if (pressLeft && spriteX > 22) {
    spriteX -= moveSpeed;
  }
  if (pressRight && spriteX < SCREEN_W - 36) {
    spriteX += moveSpeed;
  }

  // Efek tombol A dan B
  if (pressA) {
    spriteColor = COLOR_RED;
    playTone(880, 20); // Nada A
  } else if (pressB) {
    spriteColor = COLOR_CYAN;
    playTone(1320, 20); // Nada B
  } else {
    spriteColor = COLOR_YELLOW;
  }

  // Render ulang sprite hanya jika ada perpindahan posisi atau aksi
  if (prevSpriteX != spriteX || prevSpriteY != spriteY || pressA || pressB) {
    // Hapus posisi lama
    tft.fillRect(prevSpriteX - spriteSize / 2, prevSpriteY - spriteSize / 2, spriteSize, spriteSize, COLOR_BLACK);
    // Gambar di posisi baru
    tft.fillRect(spriteX - spriteSize / 2, spriteY - spriteSize / 2, spriteSize, spriteSize, spriteColor);
    tft.drawRect(spriteX - spriteSize / 2, spriteY - spriteSize / 2, spriteSize, spriteSize, COLOR_WHITE);
  }

  // =========================================================================
  // 4. VISUALISASI STATUS CONTROLLER (GAMEPAD HUD)
  // =========================================================================
  int hudY = 200;

  // Kotak Virtual D-Pad
  tft.fillRect(20, hudY, 18, 14, pressLeft  ? COLOR_GREEN : COLOR_DARKGREY);
  tft.fillRect(38, hudY - 14, 14, 18, pressUp    ? COLOR_GREEN : COLOR_DARKGREY);
  tft.fillRect(38, hudY + 10, 14, 18, pressDown  ? COLOR_GREEN : COLOR_DARKGREY);
  tft.fillRect(48, hudY, 18, 14, pressRight ? COLOR_GREEN : COLOR_DARKGREY);

  // Tombol Virtual A & B
  tft.fillCircle(210, hudY + 4, 10, pressA ? COLOR_RED   : COLOR_DARKGREY);
  tft.fillCircle(180, hudY + 16, 10, pressB ? COLOR_BLUE : COLOR_DARKGREY);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  tft.setCursor(207, hudY);     tft.print("A");
  tft.setCursor(177, hudY + 12); tft.print("B");

  // Tombol Virtual Kontrol (MENU, VOL, SELECT, START)
  tft.fillRect(80,  hudY - 6, 20, 8, pressMenu   ? COLOR_MAGENTA : COLOR_DARKGREY);
  tft.fillRect(105, hudY - 6, 20, 8, pressVol    ? COLOR_MAGENTA : COLOR_DARKGREY);
  tft.fillRect(80,  hudY + 14, 20, 8, pressSelect ? COLOR_YELLOW  : COLOR_DARKGREY);
  tft.fillRect(105, hudY + 14, 20, 8, pressStart  ? COLOR_YELLOW  : COLOR_DARKGREY);

  tft.setCursor(82,  hudY - 16); tft.print("MEN");
  tft.setCursor(107, hudY - 16); tft.print("VOL");
  tft.setCursor(82,  hudY + 24); tft.print("SEL");
  tft.setCursor(107, hudY + 24); tft.print("STA");

  // =========================================================================
  // 5. LIVE TELEMETRY & DEBUG VALUES
  // =========================================================================
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  tft.setCursor(10, 260);
  tft.printf("ADC IO35 (V): %4d [UP/DN] ", adcVert);

  tft.setCursor(10, 275);
  tft.printf("ADC IO34 (H): %4d [LF/RT] ", adcHorz);

  tft.setCursor(10, 292);
  tft.setTextColor(COLOR_GREEN, COLOR_BLACK);
  tft.print("Button Status: ");
  if (pressA)      tft.print("A ");
  if (pressB)      tft.print("B ");
  if (pressSelect) tft.print("SELECT ");
  if (pressStart)  tft.print("START ");
  if (pressMenu)   tft.print("MENU ");
  if (pressVol)    tft.print("VOL ");
  if (pressUp)     tft.print("UP ");
  if (pressDown)   tft.print("DOWN ");
  if (pressLeft)   tft.print("LEFT ");
  if (pressRight)  tft.print("RIGHT ");
  if (!anyInput)   tft.print("- IDLE -        ");

  delay(25); // Refresh rate ~40 FPS
}
