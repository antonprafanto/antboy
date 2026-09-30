#pragma once
#include <Arduino.h>

// =========================================================================
// ANTOS CYBERPUNK & RETRO-FUTURISTIC COLOR PALETTE (RGB565)
// =========================================================================

// Backgrounds
#define ANTOS_COLOR_BG_DARK          0x0821   // Very deep navy midnight (0, 16, 8)
#define ANTOS_COLOR_BG_PANEL         0x1082   // Card panel slate
#define ANTOS_COLOR_BG_CARD          0x18E5   // Inactive card surface
#define ANTOS_COLOR_BG_CARD_ACTIVE   0x2187   // Active focused card surface
#define ANTOS_COLOR_BG_MODAL         0x0842   // Modal overlay background

// Cyber Accents
#define ANTOS_COLOR_CYAN             0x07FF   // Neon Cyber Cyan (Primary Accent)
#define ANTOS_COLOR_YELLOW           0xFFE0   // Cyberpunk Yellow (Attention / Highlight)
#define ANTOS_COLOR_GREEN            0x07E0   // Neon Matrix Green (Status OK)
#define ANTOS_COLOR_MAGENTA          0xF81F   // Neon Pink / Magenta (Gaming)
#define ANTOS_COLOR_ORANGE           0xFD20   // Retro Orange (IoT)
#define ANTOS_COLOR_RED              0xF800   // Warning / Alert Red
#define ANTOS_COLOR_BLUE             0x001F   // Deep Blue
#define ANTOS_COLOR_GOLD             0xFEA0   // Lab / Hardware Gold

// Text & Borders
#define ANTOS_COLOR_WHITE            0xFFFF   // Bright Text
#define ANTOS_COLOR_TEXT_DIM         0x9CD3   // Dimmed secondary text
#define ANTOS_COLOR_TEXT_MUTED       0x632C   // Subtle disabled text
#define ANTOS_COLOR_BORDER_DIM       0x39E7   // Inactive frame border
#define ANTOS_COLOR_BORDER_GLOW      0x57FF   // Glowing active border

// 4 Pillar Thematic Accents
#define ANTOS_COLOR_PIL_GAMING       0xF819   // Hot Pink / Retro Purple
#define ANTOS_COLOR_PIL_WIRELESS     0x07E6   // Electric Green / Lime
#define ANTOS_COLOR_PIL_IOT          0x05DF   // Cerulean Blue / Aqua
#define ANTOS_COLOR_PIL_LAB          0xFDE0   // Amber / Cyber Gold
