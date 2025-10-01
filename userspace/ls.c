#include "commands.h"

#include "../kernel/console.h"

static const char *root_entries[] = {
    "bin",
    "etc",
    "home",
    "tmp",
    "readme.txt"
};

void userspace_cmd_ls(const char *path) {
    console_write("[ls] ");
    if (path && path[0] != '\0') {
        console_write("listing ");
        console_write_line(path);
        console_write_line("  (no entries)");
        return;
    }

    console_write_line("listing /");
    for (unsigned int i = 0; i < sizeof(root_entries) / sizeof(root_entries[0]); ++i) {
        console_write("  ");
        console_write_line(root_entries[i]);
    }
}
