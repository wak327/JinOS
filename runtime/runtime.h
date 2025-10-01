#ifndef JINOS_RUNTIME_H
#define JINOS_RUNTIME_H

#include <stdint.h>

typedef enum {
    RUNTIME_STATUS_OK = 0,
    RUNTIME_STATUS_NOT_READY = -1,
    RUNTIME_STATUS_UNSUPPORTED = -2,
    RUNTIME_STATUS_ERROR = -3
} runtime_status_t;

typedef struct {
    const char *package_name;
    const char *entry_activity;
    const char *apex_path;
} android_container_config_t;

runtime_status_t runtime_init(void);
runtime_status_t runtime_launch_android_app(const android_container_config_t *config);
runtime_status_t runtime_forward_syscall(uint64_t number, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3);

#endif /* JINOS_RUNTIME_H */
