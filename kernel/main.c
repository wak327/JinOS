#include "console.h"
#include "memory.h"
#include "scheduler.h"

#include "../drivers/display.h"
#include "../drivers/keyboard.h"
#include "../drivers/network.h"
#include "../drivers/storage.h"

#include <stddef.h>
#include <stdint.h>

extern void rust_entry(void);

static void task_alpha(void);
static void task_beta(void);

static void pump_keyboard(void) {
    keyboard_poll();
    while (keyboard_available()) {
        char ch = keyboard_read_char();
        char msg[32] = "[kbd] key=";
        size_t len = 0;
        while (msg[len] != '\0') {
            ++len;
        }
        msg[len++] = ch ? ch : '?';
        msg[len] = '\0';
        console_write_line(msg);
    }
}

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
        pump_keyboard();
        console_write_line("[task-A] yielding control");
        scheduler_yield();
    }
    console_write_line("[task-A] finishing");
    scheduler_exit_current();
}

static void task_beta(void) {
    console_write_line("[task-B] initialized");

    uint16_t *sector = (uint16_t *)kmalloc(512);
    if (sector) {
        for (int i = 0; i < 256; ++i) {
            sector[i] = (uint16_t)(0xCA00 | i);
        }
        ata_write_sector(1, sector);
        ata_read_sector(1, sector);
    }

    uint8_t packet[32];
    for (size_t i = 0; i < sizeof(packet); ++i) {
        packet[i] = (uint8_t)i;
    }
    network_send_packet(packet, sizeof(packet));
    uint8_t recv_buffer[32];
    network_receive_packet(recv_buffer, sizeof(recv_buffer));

    for (int i = 0; i < 3; ++i) {
        console_write_line("[task-B] running workload");
        scheduler_yield();
    }
    console_write_line("[task-B] finishing");
    scheduler_exit_current();
}
