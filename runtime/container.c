#include "runtime.h"

#include "../kernel/console.h"
#include "../kernel/memory.h"

static int runtime_ready = 0;

runtime_status_t runtime_init(void) {
    console_write_line("[runtime] initializing Android container stubs");

    void *context_region = kmalloc(4096);
    if (!context_region) {
        console_write_line("[runtime] failed to allocate container context");
        return RUNTIME_STATUS_ERROR;
    }

    runtime_ready = 1;
    console_write_line("[runtime] container context prepared");
    return RUNTIME_STATUS_OK;
}

runtime_status_t runtime_launch_android_app(const android_container_config_t *config) {
    if (!runtime_ready) {
        console_write_line("[runtime] launch requested before init");
        return RUNTIME_STATUS_NOT_READY;
    }
    if (!config || !config->package_name) {
        console_write_line("[runtime] invalid container configuration");
        return RUNTIME_STATUS_ERROR;
    }

    console_write("[runtime] launching Android package: ");
    console_write_line(config->package_name);
    if (config->entry_activity) {
        console_write("[runtime] activity: ");
        console_write_line(config->entry_activity);
    }
    if (config->apex_path) {
        console_write("[runtime] apex layer: ");
        console_write_line(config->apex_path);
    }

    console_write_line("[runtime] (stub) delegating to Anbox integration layer");
    return RUNTIME_STATUS_OK;
}

runtime_status_t runtime_forward_syscall(uint64_t number, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    if (!runtime_ready) {
        return RUNTIME_STATUS_NOT_READY;
    }

    console_write("[runtime] syscall #");
    console_write_hex(number);
    console_write(" args: ");
    console_write_hex(arg0);
    console_write(" ");
    console_write_hex(arg1);
    console_write(" ");
    console_write_hex(arg2);
    console_write(" ");
    console_write_hex(arg3);
    console_newline();

    console_write_line("[runtime] (stub) forward to Android container");
    (void)arg0;
    (void)arg1;
    (void)arg2;
    (void)arg3;
    return RUNTIME_STATUS_UNSUPPORTED;
}
