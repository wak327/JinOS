# jinOS Installer Toolkit

## Building an ISO

```sh
make iso
```

This invokes `installer/build_iso.sh`, which compiles the kernel and assembles a
GRUB-based bootable image at `build/jinos.iso`. Ensure `grub-mkrescue` and its
dependencies (grub, xorriso, mtools) are installed.

## Installing to Bare Metal

Write the ISO to a target disk using the helper script:

```sh
sudo installer/install.sh --device /dev/sdX
```

Replace `/dev/sdX` with the desired block device (USB drive, SSD, etc.). The
operation is destructive—double-check the device path.

## Updating an Installed System

The `installer/update.sh` script provides a simple package replacement
mechanism. Mount the jinOS root filesystem and apply one or more tar archives:

```sh
sudo installer/update.sh --root /mnt/jinos-root update-package.tar
```

Each archive is extracted relative to the root, overwriting existing files. This
is intended as a lightweight mechanism until a full package manager is
implemented.

## Virtualization

The generated ISO works with QEMU and VirtualBox. For a quick test:

```sh
make run
```

This boots the ISO in QEMU with serial output routed to the terminal.
