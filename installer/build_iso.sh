#!/usr/bin/env bash
# Build a bootable jinOS ISO image using grub-mkrescue.
set -euo pipefail

SCRIPT_DIR=$(cd -- "$(dirname "${BASH_SOURCE[0]}")" && pwd)
ROOT_DIR=$(cd -- "${SCRIPT_DIR}/.." && pwd)
BUILD_DIR="${ROOT_DIR}/build"
ISO_DIR="${BUILD_DIR}/iso"
KERNEL_ELF="${BUILD_DIR}/kernel.elf"
ISO_IMAGE="${BUILD_DIR}/jinos.iso"

mkdir -p "${BUILD_DIR}"

make -C "${ROOT_DIR}" all

rm -rf "${ISO_DIR}"
mkdir -p "${ISO_DIR}/boot/grub"

cp "${KERNEL_ELF}" "${ISO_DIR}/boot/kernel.elf"
cp "${ROOT_DIR}/bootloader/grub.cfg" "${ISO_DIR}/boot/grub/grub.cfg"

grub-mkrescue -o "${ISO_IMAGE}" "${ISO_DIR}"

echo "[installer] ISO generated at ${ISO_IMAGE}"
