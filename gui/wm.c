#include "gui.h"

#include "../kernel/console.h"

#define GUI_MAX_WINDOWS 8
#define TITLE_MAX 32

typedef struct {
    int in_use;
    char title[TITLE_MAX];
    uint16_t width;
    uint16_t height;
    uint16_t x;
    uint16_t y;
} gui_window_t;

static gui_window_t windows[GUI_MAX_WINDOWS];
static gui_window_handle focused_window = -1;
static unsigned int frame_counter = 0;

static void copy_title(char *dest, const char *src) {
    unsigned int i = 0;
    if (!src) {
        dest[0] = '\0';
        return;
    }
    while (src[i] != '\0' && i + 1U < TITLE_MAX) {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

void gui_wm_init(void) {
    for (int i = 0; i < GUI_MAX_WINDOWS; ++i) {
        windows[i].in_use = 0;
    }
    focused_window = -1;
    frame_counter = 0;
    console_write_line("[gui] window manager online");
}

gui_window_handle gui_wm_create_window(const char *title, uint16_t width, uint16_t height, uint16_t x, uint16_t y) {
    for (int i = 0; i < GUI_MAX_WINDOWS; ++i) {
        if (!windows[i].in_use) {
            windows[i].in_use = 1;
            copy_title(windows[i].title, title);
            windows[i].width = width;
            windows[i].height = height;
            windows[i].x = x;
            windows[i].y = y;

            console_write("[gui] window created: ");
            console_write_line(windows[i].title);
            if (focused_window < 0) {
                focused_window = i;
                console_write("[gui] focus set to: ");
                console_write_line(windows[i].title);
            }
            return i;
        }
    }

    console_write_line("[gui] window creation failed: limit reached");
    return -1;
}

void gui_wm_move_window(gui_window_handle handle, uint16_t x, uint16_t y) {
    if (handle < 0 || handle >= GUI_MAX_WINDOWS) {
        return;
    }
    gui_window_t *window = &windows[handle];
    if (!window->in_use) {
        return;
    }

    window->x = x;
    window->y = y;
    console_write("[gui] window moved: ");
    console_write_line(window->title);
}

void gui_wm_focus_window(gui_window_handle handle) {
    if (handle < 0 || handle >= GUI_MAX_WINDOWS) {
        return;
    }
    gui_window_t *window = &windows[handle];
    if (!window->in_use) {
        return;
    }

    focused_window = handle;
    console_write("[gui] window focused: ");
    console_write_line(window->title);
}

void gui_wm_render_frame(void) {
    frame_counter++;
    if ((frame_counter % 240U) != 0U) {
        return;
    }

    console_write_line("[gui] compositor frame");
    for (int i = 0; i < GUI_MAX_WINDOWS; ++i) {
        if (!windows[i].in_use) {
            continue;
        }
        console_write("  [win] ");
        console_write(windows[i].title);
        if (i == focused_window) {
            console_write(" (focused)");
        }
        console_newline();
    }
}
