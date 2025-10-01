#include <stddef.h>
#include <stdint.h>

extern void rust_entry(void);

static volatile uint16_t *const VGA_BUFFER = (volatile uint16_t *)0xB8000;
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)color << 8 | (uint16_t)c;
}

static void vga_clear(uint8_t color) {
    for (size_t y = 0; y < VGA_HEIGHT; ++y) {
        for (size_t x = 0; x < VGA_WIDTH; ++x) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', color);
        }
    }
}

static void vga_write_string(const char *str, uint8_t row, uint8_t col, uint8_t color) {
    size_t index = (size_t)row * VGA_WIDTH + col;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        VGA_BUFFER[index + i] = vga_entry(str[i], color);
    }
}

void kernel_main(void) {
    const uint8_t text_color = 0x0F; /* white on black */
    vga_clear(0x00);
    vga_write_string("jinOs Kernel Started", 0, 0, text_color);
    rust_entry();

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
