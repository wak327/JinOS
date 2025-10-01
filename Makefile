BUILD_DIR := build
ISO_DIR := $(BUILD_DIR)/iso
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
ISO_IMAGE := $(BUILD_DIR)/jinos.iso

.PHONY: all kernel iso run clean

all: $(KERNEL_ELF)

$(KERNEL_ELF):
	$(MAKE) -C kernel all

iso: all
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	cp bootloader/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_IMAGE) $(ISO_DIR)

run: iso
	qemu-system-x86_64 -cdrom $(ISO_IMAGE) -serial stdio

clean:
	$(MAKE) -C kernel clean
	rm -rf $(BUILD_DIR)
