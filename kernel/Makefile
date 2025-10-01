BUILD_DIR := ../build
KERNEL_ELF := $(BUILD_DIR)/kernel.elf

CC ?= clang
LD ?= ld
RUSTC ?= rustc

CFLAGS := -Wall -Wextra -std=c11 -ffreestanding -fno-stack-protector -fno-pic -m64 -mno-red-zone
ASFLAGS := -ffreestanding -fno-pic -m64 -mno-red-zone
LDFLAGS := -nostdlib -z max-page-size=0x1000 -T linker.ld
RUSTFLAGS := --target x86_64-unknown-none -C panic=abort -O --crate-type staticlib

C_SOURCES := main.c console.c memory.c scheduler.c
C_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(BUILD_DIR)/boot.o $(BUILD_DIR)/context_switch.o
OBJS := $(ASM_OBJECTS) $(C_OBJECTS)

.PHONY: all clean

all: $(KERNEL_ELF)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/boot.o: ../bootloader/boot.S | $(BUILD_DIR)
	$(CC) -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/context_switch.o: context_switch.S | $(BUILD_DIR)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/librust.a: lib.rs | $(BUILD_DIR)
	$(RUSTC) $(RUSTFLAGS) $< -o $@

$(KERNEL_ELF): linker.ld $(OBJS) $(BUILD_DIR)/librust.a
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(BUILD_DIR)/librust.a

clean:
	rm -f $(OBJS) $(BUILD_DIR)/librust.a $(KERNEL_ELF)
