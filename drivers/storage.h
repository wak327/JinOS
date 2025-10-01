#ifndef JINOS_DRIVERS_STORAGE_H
#define JINOS_DRIVERS_STORAGE_H

#include <stddef.h>
#include <stdint.h>

void storage_init(void);
int ata_read_sector(uint32_t lba, void *buffer);
int ata_write_sector(uint32_t lba, const void *buffer);

#endif /* JINOS_DRIVERS_STORAGE_H */
