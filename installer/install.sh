#!/usr/bin/env bash
# Install jinOS onto a target block device (e.g. /dev/sdX).
# WARNING: This will destroy data on the specified device.
set -euo pipefail

usage() {
    cat <<USAGE
Usage: $0 --device <block-device> [--iso <path-to-iso>]

Examples:
  sudo $0 --device /dev/sdX
  sudo $0 --device /dev/sdX --iso build/jinos.iso

The script will write the jinOS ISO image to the device using dd.
USAGE
}

DEVICE=""
ISO_IMAGE=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --device)
            DEVICE="$2"
            shift 2
            ;;
        --iso)
            ISO_IMAGE="$2"
            shift 2
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            echo "[installer] unknown argument: $1" >&2
            usage
            exit 1
            ;;
    esac
done

if [[ -z "${DEVICE}" ]]; then
    echo "[installer] --device is required" >&2
    usage
    exit 1
fi

if [[ ! -b "${DEVICE}" ]]; then
    echo "[installer] ${DEVICE} is not a block device" >&2
    exit 1
fi

SCRIPT_DIR=$(cd -- "$(dirname "${BASH_SOURCE[0]}")" && pwd)
ROOT_DIR=$(cd -- "${SCRIPT_DIR}/.." && pwd)

if [[ -z "${ISO_IMAGE}" ]]; then
    ISO_IMAGE="${ROOT_DIR}/build/jinos.iso"
fi

if [[ ! -f "${ISO_IMAGE}" ]]; then
    echo "[installer] ISO image not found at ${ISO_IMAGE}" >&2
    echo "           Run 'make iso' first." >&2
    exit 1
fi

echo "[installer] Writing ${ISO_IMAGE} to ${DEVICE}" >&2
sudo dd if="${ISO_IMAGE}" of="${DEVICE}" bs=4M status=progress conv=fsync

echo "[installer] Installation complete. You may now boot from ${DEVICE}."
