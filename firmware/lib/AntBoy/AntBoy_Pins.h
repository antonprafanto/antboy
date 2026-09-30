#pragma once
#include <Arduino.h>

// =========================================================================
// ANTBOY (2026) OFFICIAL PINOUT DEFINITION (KiCad PCB Baseline V1)
// =========================================================================

// --- Display Subsystem (ST7789 2.0" IPS 320x240) ---
#define ANTBOY_PIN_TFT_MOSI        23   // VSPI MOSI (Shared with SD Card & J4 Pin 10)
#define ANTBOY_PIN_TFT_SCK         18   // VSPI SCK  (Shared with SD Card & J4 Pin 6)
#define ANTBOY_PIN_TFT_CS           5   // LCD Chip Select (Active-LOW, Shared J4 Pin 2)
#define ANTBOY_PIN_TFT_DC          21   // Data/Command Selector (Shared J4 Pin 8)
#define ANTBOY_PIN_TFT_RST         -1   // Hardwired to ESP32 Hardware Reset (EN)
#define ANTBOY_PIN_TFT_BLK         14   // Backlight PWM (Active-HIGH, Shared J4 Pin 4)

// --- MicroSD Card Storage Subsystem (SPI) ---
#define ANTBOY_PIN_SD_CS           22   // SD Card Chip Select (Active-LOW, Shared J4 Pin 9)
#define ANTBOY_PIN_SD_MOSI         23   // VSPI MOSI (Shared with TFT)
#define ANTBOY_PIN_SD_SCK          18   // VSPI SCK  (Shared with TFT)
#define ANTBOY_PIN_SD_MISO         19   // VSPI MISO (Shared J4 Pin 7)

// --- Audio & Status Indication ---
#define ANTBOY_PIN_BUZZER          26   // Passive Piezo Buzzer (Series R1 1kΩ, Shared J4 Pin 12)
#define ANTBOY_PIN_LED              2   // Status Green LED D1 (Series R10 10kΩ to GND)

// --- Action & Function Buttons (Active-LOW Digital Inputs) ---
#define ANTBOY_PIN_BTN_A           33   // Button A (Internal Pull-Up, Shared J4 Pin 15)
#define ANTBOY_PIN_BTN_B           32   // Button B (Internal Pull-Up, Shared J4 Pin 14)
#define ANTBOY_PIN_BTN_SELECT      27   // Button SELECT (Internal Pull-Up, Shared J4 Pin 13)
#define ANTBOY_PIN_BTN_START       39   // Button START (Input-Only VN, Pull-Up Eksternal R9 10k)
#define ANTBOY_PIN_BTN_MENU        13   // Button MENU (Internal Pull-Up, Shared J4 Pin 3)
#define ANTBOY_PIN_BTN_VOL          0   // Button VOL (Boot Pin, Pull-Up Eksternal R3 10k)

// --- D-Pad Directional Buttons (Dual-Channel Resistor Ladder ADC) ---
#define ANTBOY_PIN_DPAD_VERT       35   // Vertical ADC: UP (~4095) / DOWN (~2048)
#define ANTBOY_PIN_DPAD_HORZ       34   // Horizontal ADC: LEFT (~4095) / RIGHT (~2048)

// --- Side Expansion Header J4 (15-Pin) Isolated / Conflict-Free Pins ---
#define ANTBOY_PIN_EXP_IO4          4   // 100% Free: I2C SDA / Touch 0 / ADC2_CH0
#define ANTBOY_PIN_EXP_IO16        16   // 100% Free: I2C SCL / UART2 RX2 / 1-Wire
#define ANTBOY_PIN_EXP_IO25        25   // 100% Free: DAC1 True 8-bit Analog / Servo PWM
