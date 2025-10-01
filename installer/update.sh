#!/usr/bin/env bash
# Simple package replacement updater for jinOS installations.
set -euo pipefail

usage() {
    cat <<USAGE
Usage: $0 --root <mount-point> <archive1.tar> [archive2.tar ...]

Each archive is extracted into the specified root filesystem. Archives should
contain absolute-style paths (e.g. usr/bin/app) relative to the root.
USAGE
}

if [[ $# -lt 3 ]]; then
    usage
    exit 1
fi

if [[ "$1" != "--root" ]]; then
    usage
    exit 1
fi
shift
ROOT="${1}"
shift

if [[ ! -d "${ROOT}" ]]; then
    echo "[update] root path ${ROOT} does not exist" >&2
    exit 1
fi

for ARCHIVE in "$@"; do
    if [[ ! -f "${ARCHIVE}" ]]; then
        echo "[update] archive ${ARCHIVE} missing" >&2
        exit 1
    fi

    echo "[update] applying ${ARCHIVE}"
    tar -xpf "${ARCHIVE}" -C "${ROOT}"
done

echo "[update] update complete"
