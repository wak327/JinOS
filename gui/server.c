#include "gui.h"

#include "../kernel/console.h"

typedef struct {
    int running;
    unsigned int tick_count;
} gui_server_state_t;

static gui_server_state_t server_state = {0, 0};

int gui_server_init(void) {
    if (server_state.running) {
        console_write_line("[gui] server already initialized");
        return 0;
    }

    console_write_line("[gui] Wayland-like display server booting");
    server_state.running = 1;
    server_state.tick_count = 0;
    console_write_line("[gui] registry: compositor, shell, xdg_wm_base");
    return 0;
}

void gui_server_process_events(void) {
    if (!server_state.running) {
        return;
    }

    server_state.tick_count++;
    if ((server_state.tick_count % 200U) == 0U) {
        console_write_line("[gui] server: dispatching noop frame events");
    }
}
