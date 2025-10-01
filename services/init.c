#include "services.h"

#include "../kernel/console.h"
#include "../kernel/scheduler.h"

void service_init_daemon_entry(void) {
    console_write_line("[svc-init] starting system services");

    if (scheduler_create_process(logging_daemon_entry, "logd") != 0) {
        console_write_line("[svc-init] failed to spawn logd");
    } else {
        console_write_line("[svc-init] logd spawned");
    }

    if (scheduler_create_process(networking_daemon_entry, "netd") != 0) {
        console_write_line("[svc-init] failed to spawn netd");
    } else {
        console_write_line("[svc-init] netd spawned");
    }

    console_write_line("[svc-init] entering supervising loop");
    for (;;) {
        scheduler_yield();
    }
}
