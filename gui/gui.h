#ifndef JINOS_GUI_H
#define JINOS_GUI_H

#include <stdint.h>

typedef int gui_window_handle;

int gui_server_init(void);
void gui_server_process_events(void);

void gui_wm_init(void);
gui_window_handle gui_wm_create_window(const char *title, uint16_t width, uint16_t height, uint16_t x, uint16_t y);
void gui_wm_move_window(gui_window_handle handle, uint16_t x, uint16_t y);
void gui_wm_focus_window(gui_window_handle handle);
void gui_wm_render_frame(void);

void gui_desktop_app_init(void);
void gui_desktop_app_update(void);

void gui_process_entry(void);

#endif /* JINOS_GUI_H */
