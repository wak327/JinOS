#include "console.h"
#include "memory.h"
#include "scheduler.h"

#include <stdint.h>

extern void rust_entry(void);

static void task_alpha(void);
static void task_beta(void);

void kernel_main(void) {
    console_init();
    console_write_line("jinOs Kernel Started");

    rust_entry();
    console_newline();

    paging_init();
    heap_init();

    scheduler_init();
    if (scheduler_create_process(task_alpha, "task-A") != 0) {
        console_write_line("[kernel] failed to create task-A");
    }
    if (scheduler_create_process(task_beta, "task-B") != 0) {
        console_write_line("[kernel] failed to create task-B");
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

static void task_alpha(void) {
    console_write_line("[task-A] initialized");
    for (int i = 0; i < 3; ++i) {
        console_write_line("[task-A] yielding control");
        scheduler_yield();
    }
    console_write_line("[task-A] finishing");
    scheduler_exit_current();
}

static void task_beta(void) {
    console_write_line("[task-B] initialized");
    for (int i = 0; i < 3; ++i) {
        console_write_line("[task-B] running workload");
        scheduler_yield();
    }
    console_write_line("[task-B] finishing");
    scheduler_exit_current();
}
