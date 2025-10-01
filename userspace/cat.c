#include "commands.h"

#include "../kernel/console.h"

#include <stddef.h>

typedef struct {
    const char *name;
    const char *content;
} file_entry_t;

static const file_entry_t files[] = {
    {"readme.txt", "jinOS demonstration file\nThis is a placeholder for future filesystem data."},
    {"motd", "Welcome to jinOS!"}
};

static const file_entry_t *find_file(const char *name) {
    unsigned int count = sizeof(files) / sizeof(files[0]);
    for (unsigned int i = 0; i < count; ++i) {
        const char *candidate = files[i].name;
        unsigned int index = 0;
        while (candidate[index] != '\0' && name[index] != '\0' && candidate[index] == name[index]) {
            ++index;
        }
        if (candidate[index] == '\0' && name[index] == '\0') {
            return &files[i];
        }
    }
    return NULL;
}

void userspace_cmd_cat(const char *path) {
    if (!path || path[0] == '\0') {
        console_write_line("[cat] usage: cat <file>");
        return;
    }

    const file_entry_t *file = find_file(path);
    if (!file) {
        console_write("[cat] file not found: ");
        console_write_line(path);
        return;
    }

    console_write_line(file->content);
}
