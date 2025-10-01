#ifndef JINOS_DRIVERS_KEYBOARD_H
#define JINOS_DRIVERS_KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
void keyboard_poll(void);
int keyboard_available(void);
char keyboard_read_char(void);

#endif /* JINOS_DRIVERS_KEYBOARD_H */
