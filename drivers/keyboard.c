#include "keyboard.h"

#include "../kernel/console.h"

#include <stdbool.h>

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 128

static volatile char key_buffer[KEYBOARD_BUFFER_SIZE];
static volatile uint8_t buffer_head = 0;
static volatile uint8_t buffer_tail = 0;
static bool shift_active = false;

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static char translate_scancode(uint8_t scancode) {
    static const char scancode_set1[128] = {
        /* 0x00 - 0x0F */
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
        /* 0x10 - 0x1F */
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
        /* 0x20 - 0x2F */
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
        /* 0x30 - 0x3F */
        'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
        /* 0x40 - 0x4F */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /* 0x50 - 0x5F */
        0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
        /* 0x60 - 0x6F */
        '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /* 0x70 - 0x7F */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    char ch = 0;
    if (scancode < 128) {
        ch = scancode_set1[scancode];
        if (shift_active && ch >= 'a' && ch <= 'z') {
            ch = (char)(ch - 'a' + 'A');
        }
        if (shift_active) {
            switch (ch) {
                case '1': ch = '!'; break;
                case '2': ch = '@'; break;
                case '3': ch = '#'; break;
                case '4': ch = '$'; break;
                case '5': ch = '%'; break;
                case '6': ch = '^'; break;
                case '7': ch = '&'; break;
                case '8': ch = '*'; break;
                case '9': ch = '('; break;
                case '0': ch = ')'; break;
                case '-': ch = '_'; break;
                case '=': ch = '+'; break;
                case '[': ch = '{'; break;
                case ']': ch = '}'; break;
                case ';': ch = ':'; break;
                case '\'': ch = '"'; break;
                case ',': ch = '<'; break;
                case '.': ch = '>'; break;
                case '/': ch = '?'; break;
                default: break;
            }
        }
    }
    return ch;
}

static void buffer_push(char ch) {
    uint8_t next_head = (uint8_t)((buffer_head + 1) % KEYBOARD_BUFFER_SIZE);
    if (next_head == buffer_tail) {
        buffer_tail = (uint8_t)((buffer_tail + 1) % KEYBOARD_BUFFER_SIZE);
    }
    key_buffer[buffer_head] = ch;
    buffer_head = next_head;
}

static void handle_scan_code(uint8_t scancode) {
    const uint8_t RELEASE_MASK = 0x80;
    const uint8_t LEFT_SHIFT = 0x2A;
    const uint8_t RIGHT_SHIFT = 0x36;

    if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) {
        shift_active = true;
        return;
    }
    if (scancode == (LEFT_SHIFT | RELEASE_MASK) || scancode == (RIGHT_SHIFT | RELEASE_MASK)) {
        shift_active = false;
        return;
    }

    if (scancode & RELEASE_MASK) {
        return;
    }

    char ch = translate_scancode(scancode);
    if (ch) {
        buffer_push(ch);
    }
}

void keyboard_init(void) {
    shift_active = false;
    buffer_head = 0;
    buffer_tail = 0;

    outb(PS2_STATUS_PORT, 0xAE); /* enable first PS/2 port */
    outb(PS2_DATA_PORT, 0xF4);   /* enable scanning */
    console_write_line("[kbd] PS/2 keyboard online");
}

void keyboard_poll(void) {
    while (inb(PS2_STATUS_PORT) & 0x01) {
        uint8_t scancode = inb(PS2_DATA_PORT);
        handle_scan_code(scancode);
    }
}

int keyboard_available(void) {
    return buffer_head != buffer_tail;
}

char keyboard_read_char(void) {
    if (!keyboard_available()) {
        return 0;
    }
    char ch = key_buffer[buffer_tail];
    buffer_tail = (uint8_t)((buffer_tail + 1) % KEYBOARD_BUFFER_SIZE);
    return ch;
}
