#ifndef JINOS_SCHEDULER_H
#define JINOS_SCHEDULER_H

#include <stddef.h>

typedef void (*process_entry_t)(void);

void scheduler_init(void);
int scheduler_create_process(process_entry_t entry, const char *name);
void scheduler_run(void);
void scheduler_yield(void);
__attribute__((noreturn)) void scheduler_exit_current(void);

#endif /* JINOS_SCHEDULER_H */
