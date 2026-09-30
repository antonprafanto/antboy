/**
 * Peanut-GB: A tidy Game Boy emulator header-only library for microcontrollers.
 * Author: Mahyar Koshkouei
 * License: MIT
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_WIDTH   160
#define LCD_HEIGHT  144

enum gb_error_e {
    GB_UNKNOWN_ERROR = 0,
    GB_INVALID_OPCODE,
    GB_INVALID_READ,
    GB_INVALID_WRITE,
    GB_HALT_FOREVER
};

enum gb_init_error_e {
    GB_INIT_NO_ERROR = 0,
    GB_INIT_CARTRIDGE_UNSUPPORTED,
    GB_INIT_INVALID_CHECKSUM
};

struct gb_s;

typedef uint8_t (*gb_rom_read_fn)(struct gb_s *gb, const uint32_t addr);
typedef uint8_t (*gb_cart_ram_read_fn)(struct gb_s *gb, const uint32_t addr);
typedef void (*gb_cart_ram_write_fn)(struct gb_s *gb, const uint32_t addr, const uint8_t val);
typedef void (*gb_error_fn)(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t addr);
typedef void (*gb_lcd_draw_line_fn)(struct gb_s *gb, const uint8_t pixels[160], const uint_fast8_t line);

struct gb_registers_s {
    union {
        struct { uint8_t f; uint8_t a; } bytes;
        uint16_t reg;
    } af;
    union {
        struct { uint8_t c; uint8_t b; } bytes;
        uint16_t reg;
    } bc;
    union {
        struct { uint8_t e; uint8_t d; } bytes;
        uint16_t reg;
    } de;
    union {
        struct { uint8_t l; uint8_t h; } bytes;
        uint16_t reg;
    } hl;
    uint16_t sp;
    uint16_t pc;
};

struct gb_joypad_s {
    union {
        struct {
            unsigned int a : 1;
            unsigned int b : 1;
            unsigned int select : 1;
            unsigned int start : 1;
            unsigned int right : 1;
            unsigned int left : 1;
            unsigned int up : 1;
            unsigned int down : 1;
        } bits;
        uint8_t val;
    };
};

struct gb_s {
    gb_rom_read_fn gb_rom_read;
    gb_cart_ram_read_fn gb_cart_ram_read;
    gb_cart_ram_write_fn gb_cart_ram_write;
    gb_error_fn gb_error;
    gb_lcd_draw_line_fn display_draw_line;

    struct gb_registers_s cpu_reg;
    struct gb_joypad_s joypad;

    uint8_t wram[8192];
    uint8_t vram[8192];
    uint8_t hram[128];
    uint8_t oam[160];

    // MBC
    uint8_t mbc;
    uint8_t cart_ram_bank;
    uint8_t cart_rom_bank;
    bool cart_ram_enable;
    bool mbc1_mode;

    // Interrupts
    uint8_t interrupt_enable;
    uint8_t interrupt_flag;
    bool ime;

    // Timers
    uint16_t div_counter;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;
    uint32_t tima_counter;

    // LCD
    uint8_t lcdc;
    uint8_t stat;
    uint8_t scy;
    uint8_t scx;
    uint8_t ly;
    uint8_t lyc;
    uint8_t dma;
    uint8_t bgp;
    uint8_t obp0;
    uint8_t obp1;
    uint8_t wy;
    uint8_t wx;
    uint32_t lcd_counter;
    bool lcd_blank;

    // Direct context pointer for user app
    void *direct;
};

enum gb_init_error_e gb_init(struct gb_s *gb,
                             gb_rom_read_fn rom_read,
                             gb_cart_ram_read_fn ram_read,
                             gb_cart_ram_write_fn ram_write,
                             gb_error_fn error_fn,
                             void *direct);

void gb_init_lcd(struct gb_s *gb, gb_lcd_draw_line_fn draw_line_fn);
void gb_run_frame(struct gb_s *gb);
void gb_step_cpu(struct gb_s *gb);

#ifdef __cplusplus
}
#endif
