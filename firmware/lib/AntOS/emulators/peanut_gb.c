#include "peanut_gb.h"

#define VBLANK_INTR   0x01
#define LCDC_INTR     0x02
#define TIMER_INTR    0x04
#define SERIAL_INTR   0x08
#define CONTROL_INTR  0x10

static uint8_t gb_read_byte(struct gb_s *gb, const uint16_t addr) {
    if (addr < 0x4000) {
        return gb->gb_rom_read(gb, addr);
    } else if (addr < 0x8000) {
        uint32_t rom_addr = ((uint32_t)gb->cart_rom_bank * 0x4000) + (addr - 0x4000);
        return gb->gb_rom_read(gb, rom_addr);
    } else if (addr < 0xA000) {
        return gb->vram[addr - 0x8000];
    } else if (addr < 0xC000) {
        if (gb->cart_ram_enable && gb->gb_cart_ram_read) {
            uint32_t ram_addr = ((uint32_t)gb->cart_ram_bank * 0x2000) + (addr - 0xA000);
            return gb->gb_cart_ram_read(gb, ram_addr);
        }
        return 0xFF;
    } else if (addr < 0xE000) {
        return gb->wram[addr - 0xC000];
    } else if (addr < 0xFE00) {
        return gb->wram[addr - 0xE000]; // Echo RAM
    } else if (addr < 0xFEA0) {
        return gb->oam[addr - 0xFE00];
    } else if (addr >= 0xFF80 && addr < 0xFFFF) {
        return gb->hram[addr - 0xFF80];
    }

    // IO Registers
    switch (addr) {
        case 0xFF00: { // Joypad
            uint8_t res = 0xCF;
            if (!(gb->joypad.val & 0x10)) { // P14 Direction keys
                res &= (gb->joypad.bits.right ? 0xFE : 0xFF);
                res &= (gb->joypad.bits.left  ? 0xFD : 0xFF);
                res &= (gb->joypad.bits.up    ? 0xFB : 0xFF);
                res &= (gb->joypad.bits.down  ? 0xF7 : 0xFF);
            }
            if (!(gb->joypad.val & 0x20)) { // P15 Action keys
                res &= (gb->joypad.bits.a      ? 0xFE : 0xFF);
                res &= (gb->joypad.bits.b      ? 0xFD : 0xFF);
                res &= (gb->joypad.bits.select ? 0xFB : 0xFF);
                res &= (gb->joypad.bits.start  ? 0xF7 : 0xFF);
            }
            return res;
        }
        case 0xFF04: return (gb->div_counter >> 8);
        case 0xFF05: return gb->tima;
        case 0xFF06: return gb->tma;
        case 0xFF07: return gb->tac;
        case 0xFF0F: return gb->interrupt_flag | 0xE0;
        case 0xFF40: return gb->lcdc;
        case 0xFF41: return gb->stat | 0x80;
        case 0xFF42: return gb->scy;
        case 0xFF43: return gb->scx;
        case 0xFF44: return gb->ly;
        case 0xFF45: return gb->lyc;
        case 0xFF47: return gb->bgp;
        case 0xFF48: return gb->obp0;
        case 0xFF49: return gb->obp1;
        case 0xFF4A: return gb->wy;
        case 0xFF4B: return gb->wx;
        case 0xFFFF: return gb->interrupt_enable;
        default: return 0xFF;
    }
}

static void gb_write_byte(struct gb_s *gb, const uint16_t addr, const uint8_t val) {
    if (addr < 0x2000) {
        gb->cart_ram_enable = ((val & 0x0F) == 0x0A);
    } else if (addr < 0x4000) {
        uint8_t bank = val & 0x1F;
        if (bank == 0) bank = 1;
        gb->cart_rom_bank = (gb->cart_rom_bank & 0x60) | bank;
    } else if (addr < 0x6000) {
        if (gb->mbc1_mode) {
            gb->cart_ram_bank = val & 0x03;
        } else {
            gb->cart_rom_bank = (gb->cart_rom_bank & 0x1F) | ((val & 0x03) << 5);
        }
    } else if (addr < 0x8000) {
        gb->mbc1_mode = (val & 0x01);
    } else if (addr < 0xA000) {
        gb->vram[addr - 0x8000] = val;
    } else if (addr < 0xC000) {
        if (gb->cart_ram_enable && gb->gb_cart_ram_write) {
            uint32_t ram_addr = ((uint32_t)gb->cart_ram_bank * 0x2000) + (addr - 0xA000);
            gb->gb_cart_ram_write(gb, ram_addr, val);
        }
    } else if (addr < 0xE000) {
        gb->wram[addr - 0xC000] = val;
    } else if (addr < 0xFE00) {
        gb->wram[addr - 0xE000] = val; // Echo RAM
    } else if (addr < 0xFEA0) {
        gb->oam[addr - 0xFE00] = val;
    } else if (addr >= 0xFF80 && addr < 0xFFFF) {
        gb->hram[addr - 0xFF80] = val;
    } else {
        // IO Registers
        switch (addr) {
            case 0xFF00: gb->joypad.val = val & 0x30; break;
            case 0xFF04: gb->div_counter = 0; break;
            case 0xFF05: gb->tima = val; break;
            case 0xFF06: gb->tma = val; break;
            case 0xFF07: gb->tac = val; break;
            case 0xFF0F: gb->interrupt_flag = val & 0x1F; break;
            case 0xFF40: gb->lcdc = val; break;
            case 0xFF41: gb->stat = (gb->stat & 0x07) | (val & 0xF8); break;
            case 0xFF42: gb->scy = val; break;
            case 0xFF43: gb->scx = val; break;
            case 0xFF45: gb->lyc = val; break;
            case 0xFF46: { // DMA Transfer
                uint16_t src = (uint16_t)val << 8;
                for (int i = 0; i < 160; i++) {
                    gb->oam[i] = gb_read_byte(gb, src + i);
                }
                break;
            }
            case 0xFF47: gb->bgp = val; break;
            case 0xFF48: gb->obp0 = val; break;
            case 0xFF49: gb->obp1 = val; break;
            case 0xFF4A: gb->wy = val; break;
            case 0xFF4B: gb->wx = val; break;
            case 0xFFFF: gb->interrupt_enable = val; break;
        }
    }
}

enum gb_init_error_e gb_init(struct gb_s *gb,
                             gb_rom_read_fn rom_read,
                             gb_cart_ram_read_fn ram_read,
                             gb_cart_ram_write_fn ram_write,
                             gb_error_fn error_fn,
                             void *direct) {
    memset(gb, 0, sizeof(struct gb_s));
    gb->gb_rom_read = rom_read;
    gb->gb_cart_ram_read = ram_read;
    gb->gb_cart_ram_write = ram_write;
    gb->gb_error = error_fn;
    gb->direct = direct;

    gb->cpu_reg.af.reg = 0x01B0;
    gb->cpu_reg.bc.reg = 0x0013;
    gb->cpu_reg.de.reg = 0x00D8;
    gb->cpu_reg.hl.reg = 0x014D;
    gb->cpu_reg.sp = 0xFFFE;
    gb->cpu_reg.pc = 0x0100;

    gb->cart_rom_bank = 1;
    gb->cart_ram_bank = 0;
    gb->lcdc = 0x91;
    gb->bgp = 0xE4;
    gb->obp0 = 0xE4;
    gb->obp1 = 0xE4;

    return GB_INIT_NO_ERROR;
}

void gb_init_lcd(struct gb_s *gb, gb_lcd_draw_line_fn draw_line_fn) {
    gb->display_draw_line = draw_line_fn;
}

static void gb_render_scanline(struct gb_s *gb) {
    if (!gb->display_draw_line) return;

    uint8_t line_pixels[LCD_WIDTH];
    uint8_t ly = gb->ly;

    if (!(gb->lcdc & 0x80)) { // LCD Disabled
        memset(line_pixels, 0, sizeof(line_pixels));
        gb->display_draw_line(gb, line_pixels, ly);
        return;
    }

    // BG Tile Map Address
    uint16_t map_base = (gb->lcdc & 0x08) ? 0x9C00 : 0x9800;
    // Tile Data Base Address
    uint16_t tile_base = (gb->lcdc & 0x10) ? 0x8000 : 0x8800;
    bool signed_indices = !(gb->lcdc & 0x10);

    uint8_t y_in_bg = ly + gb->scy;
    uint8_t tile_y = y_in_bg / 8;
    uint8_t tile_y_offset = y_in_bg % 8;

    for (int x = 0; x < LCD_WIDTH; x++) {
        uint8_t x_in_bg = x + gb->scx;
        uint8_t tile_x = x_in_bg / 8;
        uint8_t tile_x_offset = x_in_bg % 8;

        uint16_t tile_addr = map_base + (tile_y * 32) + tile_x;
        uint8_t tile_idx = gb->vram[tile_addr - 0x8000];

        uint16_t data_addr;
        if (signed_indices) {
            int8_t s_idx = (int8_t)tile_idx;
            data_addr = 0x9000 + (s_idx * 16) + (tile_y_offset * 2);
        } else {
            data_addr = tile_base + (tile_idx * 16) + (tile_y_offset * 2);
        }

        uint8_t byte1 = gb->vram[data_addr - 0x8000];
        uint8_t byte2 = gb->vram[data_addr + 1 - 0x8000];

        uint8_t bit = 7 - tile_x_offset;
        uint8_t color_num = ((byte2 >> bit) & 1) << 1 | ((byte1 >> bit) & 1);

        // BGP Palette Map
        uint8_t col = (gb->bgp >> (color_num * 2)) & 3;
        line_pixels[x] = col;
    }

    gb->display_draw_line(gb, line_pixels, ly);
}

void gb_step_cpu(struct gb_s *gb) {
    // 1. Fetch next opcode
    uint8_t opcode = gb_read_byte(gb, gb->cpu_reg.pc++);
    int cycles = 4;

    switch (opcode) {
        case 0x00: break; // NOP
        case 0x01: gb->cpu_reg.bc.reg = gb_read_byte(gb, gb->cpu_reg.pc) | (gb_read_byte(gb, gb->cpu_reg.pc + 1) << 8); gb->cpu_reg.pc += 2; cycles = 12; break;
        case 0x06: gb->cpu_reg.bc.bytes.b = gb_read_byte(gb, gb->cpu_reg.pc++); cycles = 8; break;
        case 0x0E: gb->cpu_reg.bc.bytes.c = gb_read_byte(gb, gb->cpu_reg.pc++); cycles = 8; break;
        case 0x11: gb->cpu_reg.de.reg = gb_read_byte(gb, gb->cpu_reg.pc) | (gb_read_byte(gb, gb->cpu_reg.pc + 1) << 8); gb->cpu_reg.pc += 2; cycles = 12; break;
        case 0x16: gb->cpu_reg.de.bytes.d = gb_read_byte(gb, gb->cpu_reg.pc++); cycles = 8; break;
        case 0x1E: gb->cpu_reg.de.bytes.e = gb_read_byte(gb, gb->cpu_reg.pc++); cycles = 8; break;
        case 0x21: gb->cpu_reg.hl.reg = gb_read_byte(gb, gb->cpu_reg.pc) | (gb_read_byte(gb, gb->cpu_reg.pc + 1) << 8); gb->cpu_reg.pc += 2; cycles = 12; break;
        case 0x26: gb->cpu_reg.hl.bytes.h = gb_read_byte(gb, gb->cpu_reg.pc++); cycles = 8; break;
        case 0x2E: gb->cpu_reg.hl.bytes.l = gb_read_byte(gb, gb->cpu_reg.pc++); cycles = 8; break;
        case 0x31: gb->cpu_reg.sp = gb_read_byte(gb, gb->cpu_reg.pc) | (gb_read_byte(gb, gb->cpu_reg.pc + 1) << 8); gb->cpu_reg.pc += 2; cycles = 12; break;
        case 0x3E: gb->cpu_reg.af.bytes.a = gb_read_byte(gb, gb->cpu_reg.pc++); cycles = 8; break;
        case 0xC3: gb->cpu_reg.pc = gb_read_byte(gb, gb->cpu_reg.pc) | (gb_read_byte(gb, gb->cpu_reg.pc + 1) << 8); cycles = 16; break;
        case 0xCD: { // CALL nn
            uint16_t target = gb_read_byte(gb, gb->cpu_reg.pc) | (gb_read_byte(gb, gb->cpu_reg.pc + 1) << 8);
            gb->cpu_reg.pc += 2;
            gb->cpu_reg.sp -= 2;
            gb_write_byte(gb, gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
            gb_write_byte(gb, gb->cpu_reg.sp + 1, gb->cpu_reg.pc >> 8);
            gb->cpu_reg.pc = target;
            cycles = 24;
            break;
        }
        case 0xC9: { // RET
            gb->cpu_reg.pc = gb_read_byte(gb, gb->cpu_reg.sp) | (gb_read_byte(gb, gb->cpu_reg.sp + 1) << 8);
            gb->cpu_reg.sp += 2;
            cycles = 16;
            break;
        }
        case 0xE0: gb_write_byte(gb, 0xFF00 + gb_read_byte(gb, gb->cpu_reg.pc++), gb->cpu_reg.af.bytes.a); cycles = 12; break;
        case 0xF0: gb->cpu_reg.af.bytes.a = gb_read_byte(gb, 0xFF00 + gb_read_byte(gb, gb->cpu_reg.pc++)); cycles = 12; break;
        case 0xEA: { // LD (nn), A
            uint16_t dest = gb_read_byte(gb, gb->cpu_reg.pc) | (gb_read_byte(gb, gb->cpu_reg.pc + 1) << 8);
            gb->cpu_reg.pc += 2;
            gb_write_byte(gb, dest, gb->cpu_reg.af.bytes.a);
            cycles = 16;
            break;
        }
        case 0xFA: { // LD A, (nn)
            uint16_t src = gb_read_byte(gb, gb->cpu_reg.pc) | (gb_read_byte(gb, gb->cpu_reg.pc + 1) << 8);
            gb->cpu_reg.pc += 2;
            gb->cpu_reg.af.bytes.a = gb_read_byte(gb, src);
            cycles = 16;
            break;
        }
        default: cycles = 4; break;
    }

    // Timer update
    gb->div_counter += cycles;
    if (gb->tac & 0x04) {
        gb->tima_counter += cycles;
        if (gb->tima_counter >= 1024) {
            gb->tima_counter -= 1024;
            gb->tima++;
            if (gb->tima == 0) {
                gb->tima = gb->tma;
                gb->interrupt_flag |= TIMER_INTR;
            }
        }
    }

    // LCD PPU Scanline update (456 cycles per scanline)
    gb->lcd_counter += cycles;
    if (gb->lcd_counter >= 456) {
        gb->lcd_counter -= 456;
        gb->ly = (gb->ly + 1) % 154;

        if (gb->ly < LCD_HEIGHT) {
            gb_render_scanline(gb);
        } else if (gb->ly == LCD_HEIGHT) {
            gb->interrupt_flag |= VBLANK_INTR;
        }
    }

    // Interrupt handling
    if (gb->ime && (gb->interrupt_enable & gb->interrupt_flag)) {
        uint8_t fired = gb->interrupt_enable & gb->interrupt_flag;
        if (fired & VBLANK_INTR) {
            gb->ime = false;
            gb->interrupt_flag &= ~VBLANK_INTR;
            gb->cpu_reg.sp -= 2;
            gb_write_byte(gb, gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
            gb_write_byte(gb, gb->cpu_reg.sp + 1, gb->cpu_reg.pc >> 8);
            gb->cpu_reg.pc = 0x0040;
        }
    }
}

void gb_run_frame(struct gb_s *gb) {
    uint8_t cur_ly = gb->ly;
    // Step until V-Blank (line 144) is reached
    while (gb->ly >= LCD_HEIGHT) {
        gb_step_cpu(gb);
    }
    while (gb->ly < LCD_HEIGHT) {
        gb_step_cpu(gb);
    }
}
