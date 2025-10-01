#ifndef JINOS_DRIVERS_DISPLAY_H
#define JINOS_DRIVERS_DISPLAY_H

#include <stddef.h>
#include <stdint.h>

void display_init(void);
void display_clear(uint8_t color);
void display_write_text(const char *text, size_t row, size_t column, uint8_t color);
void display_putc(char c);

#endif /* JINOS_DRIVERS_DISPLAY_H */
