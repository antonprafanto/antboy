#!/usr/bin/env python3
"""
ANTBOY (2026) Asset Converter Tool: PNG/BMP to RGB565 C-Array
Usage: python tools/png_to_rgb565.py <input_image.png> <output_array_name>
"""

import sys
import os

try:
    from PIL import Image
except ImportError:
    print("Error: Pillow library required. Run 'pip install Pillow' to install.")
    sys.exit(1)

def convert_image(input_path, array_name):
    img = Image.open(input_path).convert('RGB')
    width, height = img.size

    print(f"// Image: {os.path.basename(input_path)} ({width}x{height})")
    print(f"const uint16_t {array_name}_width = {width};")
    print(f"const uint16_t {array_name}_height = {height};")
    print(f"const uint16_t {array_name}_data[] PROGMEM = {{")

    pixels = []
    for y in range(height):
        row_str = "    "
        for x in range(width):
            r, g, b = img.getpixel((x, y))
            # RGB888 to RGB565
            rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
            row_str += f"0x{rgb565:04X}, "
        print(row_str)

    print("};")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python png_to_rgb565.py <image_path> <array_name>")
        sys.exit(1)
    convert_image(sys.argv[1], sys.argv[2])
