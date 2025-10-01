BUILD_DIR := build
ISO_DIR := $(BUILD_DIR)/iso
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
ISO_IMAGE := $(BUILD_DIR)/jinos.iso

.PHONY: all kernel iso run clean

all: $(KERNEL_ELF)

$(KERNEL_ELF):
	$(MAKE) -C kernel all

iso:
	bash installer/build_iso.sh

run: iso
	qemu-system-x86_64 -cdrom $(ISO_IMAGE) -serial stdio

clean:
	$(MAKE) -C kernel clean
	rm -rf $(BUILD_DIR)
