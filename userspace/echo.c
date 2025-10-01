#include "commands.h"

#include "../kernel/console.h"

void userspace_cmd_echo(const char *text) {
    if (!text) {
        console_newline();
        return;
    }
    console_write_line(text);
}
