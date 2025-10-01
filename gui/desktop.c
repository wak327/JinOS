#include "gui.h"

#include "../kernel/console.h"
#include "../kernel/scheduler.h"

static gui_window_handle taskbar_window = -1;
static gui_window_handle launcher_window = -1;
static gui_window_handle demo_window = -1;
static unsigned int gui_tick = 0;

void gui_desktop_app_init(void) {
    console_write_line("[gui] launching Flutter/Qt desktop stub");

    taskbar_window = gui_wm_create_window("Taskbar", 800, 28, 0, 452);
    launcher_window = gui_wm_create_window("Launcher", 320, 240, 10, 200);
    demo_window = gui_wm_create_window("Notes", 400, 300, 180, 120);

    if (demo_window >= 0) {
        gui_wm_focus_window(demo_window);
    }
}

void gui_desktop_app_update(void) {
    gui_tick++;

    if (gui_tick == 300U && launcher_window >= 0) {
        gui_wm_focus_window(launcher_window);
    } else if (gui_tick == 600U && demo_window >= 0) {
        gui_wm_move_window(demo_window, 220, 140);
        gui_wm_focus_window(demo_window);
    } else if (gui_tick == 900U && launcher_window >= 0) {
        gui_wm_move_window(launcher_window, 20, 180);
    }
}

void gui_process_entry(void) {
    console_write_line("[gui] desktop process starting");

    if (gui_server_init() != 0) {
        console_write_line("[gui] server initialization failed");
        scheduler_exit_current();
    }

    gui_wm_init();
    gui_desktop_app_init();

    for (;;) {
        gui_server_process_events();
        gui_desktop_app_update();
        gui_wm_render_frame();
        scheduler_yield();
    }
}
