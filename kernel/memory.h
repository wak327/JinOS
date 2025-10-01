#ifndef JINOS_MEMORY_H
#define JINOS_MEMORY_H

#include <stddef.h>
#include <stdint.h>

void paging_init(void);
void heap_init(void);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif /* JINOS_MEMORY_H */
