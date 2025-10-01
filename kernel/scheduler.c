#include "scheduler.h"
#include "console.h"
#include "memory.h"

#include <stdint.h>

#define MAX_PROCESSES 4
#define PROCESS_NAME_MAX 16
#define PROCESS_STACK_SIZE (8 * 4096)

typedef enum {
    PROCESS_UNUSED = 0,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_FINISHED
} process_state_t;

typedef struct {
    uint64_t rbx;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rsp;
    uint64_t rip;
} context_t;

typedef struct {
    uint64_t pid;
    process_state_t state;
    context_t ctx;
    process_entry_t entry;
    void *stack;
    size_t stack_size;
    char name[PROCESS_NAME_MAX];
    size_t slot;
} process_t;

extern void context_switch(context_t *old_ctx, context_t *new_ctx);

static process_t processes[MAX_PROCESSES];
static size_t process_count = 0;
static uint64_t next_pid = 1;
static process_t *current_process = NULL;
static process_t scheduler_process;

static void set_name(process_t *proc, const char *name) {
    if (!name) {
        proc->name[0] = '\0';
        return;
    }

    size_t i = 0;
    for (; i < PROCESS_NAME_MAX - 1 && name[i] != '\0'; ++i) {
        proc->name[i] = name[i];
    }
    proc->name[i] = '\0';
}

static process_t *find_next_ready(process_t *from) {
    if (process_count == 0) {
        return NULL;
    }

    size_t start = 0;
    if (from && from->state != PROCESS_UNUSED) {
        start = from->slot;
    }

    for (size_t offset = 1; offset <= process_count; ++offset) {
        size_t index = (start + offset) % process_count;
        process_t *candidate = &processes[index];
        if (candidate->state == PROCESS_READY) {
            return candidate;
        }
    }

    if (from && from->state == PROCESS_READY) {
        return from;
    }

    return NULL;
}

static void log_switch(const char *from, const char *to) {
    console_write("[sched] ");
    if (from && from[0] != '\0') {
        console_write(from);
    } else {
        console_write("kernel");
    }
    console_write(" -> ");
    if (to && to[0] != '\0') {
        console_write(to);
    } else {
        console_write("kernel");
    }
    console_newline();
}

static void process_trampoline(void) {
    if (!current_process) {
        console_write_line("[sched] panic: trampoline without process");
        for (;;) {
            __asm__ __volatile__("hlt");
        }
    }

    current_process->entry();
    console_write("[sched] process exited: ");
    console_write(current_process->name);
    console_newline();
    scheduler_exit_current();
}

void scheduler_init(void) {
    for (size_t i = 0; i < MAX_PROCESSES; ++i) {
        processes[i].state = PROCESS_UNUSED;
        processes[i].slot = i;
    }
    process_count = 0;
    next_pid = 1;
    current_process = NULL;
    scheduler_process.state = PROCESS_RUNNING;
    scheduler_process.pid = 0;
    scheduler_process.name[0] = '\0';
}

int scheduler_create_process(process_entry_t entry, const char *name) {
    if (!entry || process_count >= MAX_PROCESSES) {
        console_write_line("[sched] unable to create process");
        return -1;
    }

    process_t *proc = &processes[process_count];
    proc->pid = next_pid++;
    proc->state = PROCESS_READY;
    proc->entry = entry;
    proc->stack_size = PROCESS_STACK_SIZE;
    proc->stack = kmalloc(proc->stack_size);
    if (!proc->stack) {
        proc->state = PROCESS_UNUSED;
        console_write_line("[sched] stack allocation failed");
        return -1;
    }

    set_name(proc, name);

    uint64_t stack_top = (uint64_t)proc->stack + proc->stack_size;
    stack_top &= ~(uint64_t)0xF;
    proc->ctx.rbx = 0;
    proc->ctx.rbp = 0;
    proc->ctx.r12 = 0;
    proc->ctx.r13 = 0;
    proc->ctx.r14 = 0;
    proc->ctx.r15 = 0;
    proc->ctx.rsp = stack_top;
    proc->ctx.rip = (uint64_t)process_trampoline;

    process_count++;
    return 0;
}

void scheduler_run(void) {
    scheduler_process.state = PROCESS_RUNNING;
    scheduler_process.ctx.rsp = 0;
    scheduler_process.ctx.rip = 0;
    current_process = &scheduler_process;

    process_t *next = find_next_ready(&scheduler_process);
    if (!next) {
        console_write_line("[sched] no runnable processes");
        return;
    }

    next->state = PROCESS_RUNNING;
    current_process = next;
    log_switch("kernel", next->name);
    context_switch(&scheduler_process.ctx, &next->ctx);

    current_process = &scheduler_process;
    console_write_line("[sched] all processes complete");
}

void scheduler_yield(void) {
    if (!current_process || current_process == &scheduler_process) {
        return;
    }

    process_t *prev = current_process;
    process_t *next = find_next_ready(prev);
    if (!next || next == prev) {
        return;
    }

    prev->state = PROCESS_READY;
    next->state = PROCESS_RUNNING;
    current_process = next;
    log_switch(prev->name, next->name);
    context_switch(&prev->ctx, &next->ctx);
    prev->state = PROCESS_RUNNING;
}

__attribute__((noreturn)) void scheduler_exit_current(void) {
    if (!current_process || current_process == &scheduler_process) {
        console_write_line("[sched] exit requested by kernel context");
        for (;;) {
            __asm__ __volatile__("hlt");
        }
    }

    process_t *exiting = current_process;
    exiting->state = PROCESS_FINISHED;

    process_t *next = find_next_ready(exiting);
    if (!next) {
        current_process = &scheduler_process;
        log_switch(exiting->name, "kernel");
        context_switch(&exiting->ctx, &scheduler_process.ctx);
    } else {
        next->state = PROCESS_RUNNING;
        current_process = next;
        log_switch(exiting->name, next->name);
        context_switch(&exiting->ctx, &next->ctx);
    }

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
