#include "console.h"

#include <stddef.h>
#include <stdint.h>

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define DEFAULT_COLOR 0x0F /* White on black */

static volatile uint16_t *const vga_buffer = (volatile uint16_t *)VGA_ADDRESS;
static uint8_t cursor_row = 0;
static uint8_t cursor_column = 0;
static uint8_t active_color = DEFAULT_COLOR;

static uint16_t vga_entry(char c, uint8_t color) {
    return ((uint16_t)color << 8) | (uint16_t)c;
}

static void clear_row(uint8_t row) {
    for (size_t column = 0; column < VGA_WIDTH; ++column) {
        vga_buffer[row * VGA_WIDTH + column] = vga_entry(' ', active_color);
    }
}

static void console_scroll(void) {
    for (size_t row = 1; row < VGA_HEIGHT; ++row) {
        for (size_t column = 0; column < VGA_WIDTH; ++column) {
            vga_buffer[(row - 1) * VGA_WIDTH + column] = vga_buffer[row * VGA_WIDTH + column];
        }
    }
    clear_row(VGA_HEIGHT - 1);
    if (cursor_row > 0) {
        cursor_row--;
    }
}

static void console_output_char(char c) {
    if (c == '\n') {
        console_newline();
        return;
    }

    vga_buffer[cursor_row * VGA_WIDTH + cursor_column] = vga_entry(c, active_color);
    cursor_column++;

    if (cursor_column >= VGA_WIDTH) {
        cursor_column = 0;
        cursor_row++;
    }

    if (cursor_row >= VGA_HEIGHT) {
        console_scroll();
    }
}

void console_newline(void) {
    cursor_column = 0;
    cursor_row++;
    if (cursor_row >= VGA_HEIGHT) {
        console_scroll();
    }
    clear_row(cursor_row);
}

void console_init(void) {
    active_color = DEFAULT_COLOR;
    cursor_row = 0;
    cursor_column = 0;
    for (size_t row = 0; row < VGA_HEIGHT; ++row) {
        clear_row((uint8_t)row);
    }
}

void console_write(const char *str) {
    if (!str) {
        return;
    }

    for (size_t i = 0; str[i] != '\0'; ++i) {
        console_output_char(str[i]);
    }
}

void console_write_line(const char *str) {
    console_write(str);
    console_newline();
}

void console_write_hex(uint64_t value) {
    char buffer[16];
    for (int i = 0; i < 16; ++i) {
        uint8_t nibble = (value >> ((15 - i) * 4)) & 0xF;
        buffer[i] = (nibble < 10) ? (char)('0' + nibble) : (char)('A' + (nibble - 10));
    }

    console_write("0x");
    char output[19];
    for (int i = 0; i < 16; ++i) {
        output[i] = buffer[i];
    }
    output[16] = '\0';
    console_write(output);
}

void console_putc(char c) {
    console_output_char(c);
}
