#include "display.h"

#define VGA_BUFFER ((volatile uint16_t *)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint8_t cursor_row = 0;
static uint8_t cursor_column = 0;
static uint8_t current_color = 0x0F; /* white on black */

static uint16_t vga_entry(char c, uint8_t color) {
    return ((uint16_t)color << 8) | (uint16_t)c;
}

static void advance_cursor(void) {
    cursor_column++;
    if (cursor_column >= VGA_WIDTH) {
        cursor_column = 0;
        cursor_row++;
    }
    if (cursor_row >= VGA_HEIGHT) {
        cursor_row = 0;
    }
}

void display_init(void) {
    cursor_row = 0;
    cursor_column = 0;
    current_color = 0x0F;
    display_clear(0x07);
}

void display_clear(uint8_t color) {
    current_color = color;
    for (size_t row = 0; row < VGA_HEIGHT; ++row) {
        for (size_t col = 0; col < VGA_WIDTH; ++col) {
            VGA_BUFFER[row * VGA_WIDTH + col] = vga_entry(' ', color);
        }
    }
    cursor_row = 0;
    cursor_column = 0;
}

void display_write_text(const char *text, size_t row, size_t column, uint8_t color) {
    if (!text) {
        return;
    }
    size_t index = row * VGA_WIDTH + column;
    while (*text && index < VGA_WIDTH * VGA_HEIGHT) {
        VGA_BUFFER[index++] = vga_entry(*text++, color);
    }
}

void display_putc(char c) {
    if (c == '\n') {
        cursor_column = 0;
        cursor_row = (uint8_t)((cursor_row + 1) % VGA_HEIGHT);
        return;
    }

    VGA_BUFFER[cursor_row * VGA_WIDTH + cursor_column] = vga_entry(c, current_color);
    advance_cursor();
}
