#include "storage.h"

#include "../kernel/console.h"

#define ATA_PRIMARY_IO 0x1F0
#define ATA_PRIMARY_CTRL 0x3F6
#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07

#define ATA_CMD_READ_SECT 0x20
#define ATA_CMD_WRITE_SECT 0x30

#define ATA_STATUS_BSY 0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_ERR 0x01

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t value;
    __asm__ __volatile__("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t value) {
    __asm__ __volatile__("outw %0, %1" : : "a"(value), "Nd"(port));
}

static int ata_wait_not_busy(void) {
    int timeout = 100000;
    while (timeout-- > 0) {
        uint8_t status = inb(ATA_PRIMARY_IO + ATA_REG_STATUS);
        if (!(status & ATA_STATUS_BSY)) {
            return 0;
        }
    }
    console_write_line("[ata] timeout waiting for BSY clear");
    return -1;
}

static int ata_wait_drq(void) {
    int timeout = 100000;
    while (timeout-- > 0) {
        uint8_t status = inb(ATA_PRIMARY_IO + ATA_REG_STATUS);
        if (status & ATA_STATUS_ERR) {
            console_write_line("[ata] device error");
            return -1;
        }
        if (status & ATA_STATUS_DRQ) {
            return 0;
        }
    }
    console_write_line("[ata] timeout waiting for DRQ");
    return -1;
}

void storage_init(void) {
    if (ata_wait_not_busy() == 0) {
        console_write_line("[ata] primary bus ready");
    }
    outb(ATA_PRIMARY_CTRL, 0x00);
}

int ata_read_sector(uint32_t lba, void *buffer) {
    if (ata_wait_not_busy() != 0) {
        return -1;
    }

    outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, (uint8_t)(0xE0 | ((lba >> 24) & 0x0F)));
    outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT0, 1);
    outb(ATA_PRIMARY_IO + ATA_REG_LBA0, (uint8_t)(lba & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_LBA1, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_LBA2, (uint8_t)((lba >> 16) & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_COMMAND, ATA_CMD_READ_SECT);

    if (ata_wait_drq() != 0) {
        return -1;
    }

    uint16_t *out = (uint16_t *)buffer;
    for (int i = 0; i < 256; ++i) {
        out[i] = inw(ATA_PRIMARY_IO + ATA_REG_DATA);
    }
    console_write_line("[ata] sector read");
    return 0;
}

int ata_write_sector(uint32_t lba, const void *buffer) {
    if (ata_wait_not_busy() != 0) {
        return -1;
    }

    outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, (uint8_t)(0xE0 | ((lba >> 24) & 0x0F)));
    outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT0, 1);
    outb(ATA_PRIMARY_IO + ATA_REG_LBA0, (uint8_t)(lba & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_LBA1, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_LBA2, (uint8_t)((lba >> 16) & 0xFF));
    outb(ATA_PRIMARY_IO + ATA_REG_COMMAND, ATA_CMD_WRITE_SECT);

    if (ata_wait_drq() != 0) {
        return -1;
    }

    const uint16_t *in = (const uint16_t *)buffer;
    for (int i = 0; i < 256; ++i) {
        outw(ATA_PRIMARY_IO + ATA_REG_DATA, in[i]);
    }
    console_write_line("[ata] sector written");
    return 0;
}
