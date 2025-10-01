#include "userspace.h"

#include "../kernel/console.h"
#include "../kernel/scheduler.h"

void init_process_entry(void) {
    console_write_line("[init] starting");

    if (scheduler_create_process(shell_process_entry, "shell") != 0) {
        console_write_line("[init] failed to spawn shell");
    } else {
        console_write_line("[init] shell spawned");
    }

    console_write_line("[init] entering idle loop");
    for (;;) {
        scheduler_yield();
    }
}
