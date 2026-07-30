#!/bin/bash
# Flash the vt-sbc-am62l-yocto build (bin/loader + bin/images, symlinked to
# build/deploy-ti/images/vt-sbc-am62l/) onto an AM62L board in USB DFU loader
# mode, using TI's ROM-DFU two-stage flashing sequence (same tool Vantron
# ships, just pointed at our own build output instead of theirs).
set -e
cd "$(dirname "$0")"

if ! command -v dfu-util >/dev/null 2>&1; then
    echo "dfu-util not found. Install it with: sudo apt install dfu-util" >&2
    exit 1
fi

if ! sudo dfu-util -l 2>/dev/null | grep -q 'Found DFU'; then
    echo "No DFU device detected." >&2
    echo "Make sure the board is powered in USB loader/DFU mode and the AM62L DFU" >&2
    echo "USB device is attached to this WSL session (usbipd attach --wsl --busid <id>)." >&2
    exit 1
fi

echo "Flashing:"
echo "  tiboot3.bin -> $(readlink -f bin/loader/tiboot3.bin)"
echo "  tispl.bin   -> $(readlink -f bin/loader/tispl.bin)"
echo "  u-boot.img  -> $(readlink -f bin/loader/u-boot.img)"
echo "  emmc.wic    -> $(readlink -f bin/images/emmc.wic)"
echo

sudo python3 dfu_flash.py "$@"
