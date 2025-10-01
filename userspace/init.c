#include "userspace.h"

#include "../kernel/console.h"
#include "../kernel/scheduler.h"
#include "../gui/gui.h"

void init_process_entry(void) {
    console_write_line("[init] starting");

    if (scheduler_create_process(shell_process_entry, "shell") != 0) {
        console_write_line("[init] failed to spawn shell");
    } else {
        console_write_line("[init] shell spawned");
    }

    if (scheduler_create_process(gui_process_entry, "gui") != 0) {
        console_write_line("[init] failed to spawn gui");
    } else {
        console_write_line("[init] gui spawned");
    }

    console_write_line("[init] entering idle loop");
    for (;;) {
        scheduler_yield();
    }
}
