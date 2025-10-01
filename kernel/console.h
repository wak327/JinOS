#ifndef JINOS_CONSOLE_H
#define JINOS_CONSOLE_H

#include <stdint.h>

void console_init(void);
void console_write(const char *str);
void console_write_line(const char *str);
void console_write_hex(uint64_t value);
void console_newline(void);
void console_putc(char c);

#endif /* JINOS_CONSOLE_H */
