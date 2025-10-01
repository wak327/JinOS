#include "userspace.h"
#include "commands.h"

#include "../drivers/keyboard.h"
#include "../kernel/console.h"
#include "../kernel/scheduler.h"

#include <stddef.h>

#define SHELL_BUFFER_SIZE 128

static int str_equals(const char *a, const char *b) {
    size_t index = 0;
    if (!a || !b) {
        return 0;
    }
    while (a[index] != '\0' && b[index] != '\0') {
        if (a[index] != b[index]) {
            return 0;
        }
        ++index;
    }
    return a[index] == '\0' && b[index] == '\0';
}

static void skip_spaces(const char **cursor) {
    while (**cursor == ' ' || **cursor == '\t') {
        ++(*cursor);
    }
}

static size_t read_token(const char **cursor, char *destination, size_t capacity) {
    size_t written = 0;
    skip_spaces(cursor);
    while (**cursor != '\0' && **cursor != ' ' && **cursor != '\t') {
        if (written + 1 < capacity) {
            destination[written++] = **cursor;
        }
        ++(*cursor);
    }
    destination[written] = '\0';
    skip_spaces(cursor);
    return written;
}

static int shell_read_line(char *buffer, size_t capacity) {
    size_t position = 0;
    for (;;) {
        keyboard_poll();
        if (!keyboard_available()) {
            scheduler_yield();
            continue;
        }

        char ch = keyboard_read_char();
        if (ch == '\r') {
            ch = '\n';
        }

        if (ch == '\n') {
            console_newline();
            break;
        }

        if (ch == '\b') {
            if (position > 0) {
                --position;
                console_write("\b \b");
            }
            continue;
        }

        if (ch >= 32 && ch < 127) {
            if (position + 1 < capacity) {
                buffer[position++] = ch;
                console_putc(ch);
            }
        }
    }

    buffer[position] = '\0';
    return (int)position;
}

static void shell_execute(const char *line) {
    const char *cursor = line;
    char command[16];

    skip_spaces(&cursor);
    if (*cursor == '\0') {
        return;
    }

    read_token(&cursor, command, sizeof(command));

    if (str_equals(command, "ls")) {
        char path[32];
        size_t arg_len = read_token(&cursor, path, sizeof(path));
        userspace_cmd_ls(arg_len == 0 ? NULL : path);
    } else if (str_equals(command, "cat")) {
        char target[32];
        size_t arg_len = read_token(&cursor, target, sizeof(target));
        if (arg_len == 0) {
            console_write_line("[shell] cat: missing file name");
        } else {
            userspace_cmd_cat(target);
        }
    } else if (str_equals(command, "echo")) {
        skip_spaces(&cursor);
        userspace_cmd_echo(*cursor == '\0' ? "" : cursor);
    } else if (str_equals(command, "help")) {
        console_write_line("Available commands:");
        console_write_line("  ls [path]");
        console_write_line("  cat <file>");
        console_write_line("  echo <message>");
        console_write_line("  help");
        console_write_line("  exit");
    } else if (str_equals(command, "exit")) {
        console_write_line("[shell] exiting");
        scheduler_exit_current();
    } else {
        console_write("[shell] unknown command: ");
        console_write_line(command);
    }
}

void shell_process_entry(void) {
    char line[SHELL_BUFFER_SIZE];
    console_write_line("[shell] interactive shell ready (type 'help')");

    for (;;) {
        console_write("[shell]$ ");
        int length = shell_read_line(line, sizeof(line));
        if (length < 0) {
            continue;
        }
        shell_execute(line);
    }
}
