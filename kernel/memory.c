#include "memory.h"
#include "console.h"

#include <stddef.h>
#include <stdint.h>

#define HEAP_START 0x01000000ULL
#define HEAP_SIZE  (512 * 1024ULL)

static uint8_t *heap_base = (uint8_t *)HEAP_START;
static size_t heap_offset = 0;

void paging_init(void) {
    console_write_line("[memory] paging enabled (boot identity map active)");
}

void heap_init(void) {
    heap_base = (uint8_t *)HEAP_START;
    heap_offset = 0;
    console_write_line("[memory] heap ready");
}

void *kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    size = (size + 15U) & ~((size_t)15U);
    if (heap_offset + size > HEAP_SIZE) {
        console_write_line("[memory] kmalloc: out of memory");
        return NULL;
    }

    void *address = heap_base + heap_offset;
    heap_offset += size;
    return address;
}

void kfree(void *ptr) {
    (void)ptr;
}
