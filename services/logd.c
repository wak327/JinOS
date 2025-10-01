#include "services.h"

#include "../kernel/console.h"
#include "../kernel/scheduler.h"

void logging_daemon_entry(void) {
    console_write_line("[logd] logging daemon online");
    unsigned int counter = 0;

    for (;;) {
        if ((counter % 512U) == 0U) {
            console_write("[logd] heartbeat ");
            console_write_hex(counter);
            console_newline();
        }
        counter++;
        scheduler_yield();
    }
}
