#include "console.h"
#include "memory.h"
#include "scheduler.h"

#include "../drivers/display.h"
#include "../drivers/keyboard.h"
#include "../drivers/network.h"
#include "../drivers/storage.h"
#include "../userspace/userspace.h"

#include <stddef.h>
#include <stdint.h>

extern void rust_entry(void);

void kernel_main(void) {
    display_init();
    display_write_text("jinOs display online", 0, 0, 0x1E);

    console_init();
    console_write_line("jinOs Kernel Started");

    rust_entry();
    console_newline();

    paging_init();
    heap_init();

    keyboard_init();
    storage_init();
    network_init();

    display_write_text("Drivers initialised", 1, 0, 0x1A);

    scheduler_init();
    if (scheduler_create_process(init_process_entry, "init") != 0) {
        console_write_line("[kernel] failed to create init process");
    } else {
        console_write_line("[kernel] init scheduled");
    }

    console_write_line("[kernel] launching scheduler");
    scheduler_run();
    console_write_line("[kernel] scheduler returned to kernel context");

    console_write_line("[kernel] invoking Rust subsystem");
    rust_entry();

    console_write_line("[kernel] halting");
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
