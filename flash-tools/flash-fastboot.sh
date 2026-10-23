#!/bin/sh
# Flash a vt-sbc-am62l over USB: DFU-boot the flasher U-Boot, then write the
# eMMC with fastboot. Linux/WSL counterpart of the Windows Flash Station.
#
#   1-2  ROM/SPL DFU boot of tiboot3.bin and tispl.bin   (dfu-util, stock loader)
#   3    SPL DFU-loads the flasher u-boot.img, whose bootcmd runs mmc partconf,
#        mmc bootbus and "fastboot usb 0"                 (dfu-util, u-boot-vt-flasher build)
#   4    fastboot flash mmc0boot0 boot0.img   - eMMC boot0: tiboot3 + tispl + u-boot
#   5    fastboot flash mmc0 <image>.wic.simg - eMMC user area, sparse (zero
#        blocks are skipped: ~290 MB sent instead of 506 MB, over bulk USB
#        instead of DFU's 4 KB control transfers)
#   6    fastboot reboot
#
# Needs: dfu-util, fastboot (android-sdk-platform-tools), USB access (udev
# rule or sudo). On WSL attach the board with usbipd first; it re-enumerates
# at every step, so bind it with "usbipd attach --wsl --auto-attach".
#
#   flash-fastboot.sh [-i <deploy dir>] [-l <loader dir>] [-e] [-f <step>]
#     -f  start at step <step> (1-6): after an aborted run the board is one
#         DFU stage ahead; the ROM, the R5 SPL and the A53 SPL each wait for
#         the next file rather than restarting
#     -i  directory holding vt-flasher/, boot0.img and the .wic.simg
#         (default: the build's deploy dir, via the bin/ symlinks here)
#     -l  directory holding the stock tiboot3.bin/tispl.bin used for the USB
#         boot (default: bin/loader here)
#     -e  "fastboot erase mmc0" before writing, for a zeroed device
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
IMAGES="$HERE/bin/images"            # symlinks into build/deploy-ti/images/vt-sbc-am62l
LOADER="$HERE/bin/images/vt-flasher"  # DFU-boot chain from the u-boot-vt-flasher build; -l bin/loader for the stock Vantron one
ERASE=0
FROM=1
while getopts "i:l:ef:" o; do
    case $o in
        i) IMAGES=$OPTARG ;;
        l) LOADER=$OPTARG ;;
        e) ERASE=1 ;;
        f) FROM=$OPTARG ;;
        *) exit 2 ;;
    esac
done
FLASHER="$IMAGES/vt-flasher"
SIMG=$(ls "$IMAGES"/*.rootfs.wic.simg 2>/dev/null | head -1)
BOOT0="$IMAGES/boot0.img"
for f in "$LOADER/tiboot3.bin" "$LOADER/tispl.bin" "$FLASHER/u-boot.img" "$BOOT0" "$SIMG"; do
    [ -f "$f" ] || { echo "missing: $f" >&2; exit 1; }
done
command -v dfu-util >/dev/null || { echo "dfu-util not installed" >&2; exit 1; }
command -v fastboot >/dev/null || { echo "fastboot not installed (apt install android-sdk-platform-tools)" >&2; exit 1; }

t0=$(date +%s)
step() { echo; echo "=== $(( $(date +%s) - t0 ))s  $*"; }

# dfu-util -w waits for the device to appear; -R resets it after the download
# (the ROM/SPL then boots what it just received). The exit code is not the
# verdict: U-Boot's DFU answers the final detach with "can't detach" and
# dfu-util exits non-zero after a complete transfer. "Download done" is.
dfu_dl() {
    rc=0; out=$(dfu-util -w -R -a "$1" -D "$2" 2>&1) || rc=$?
    echo "$out" | grep -vE "^(Copyright|This program|Please report|dfu-util: Warning: Invalid DFU suffix|dfu-util: A valid DFU suffix|$)"
    echo "$out" | grep -q "Download done" || { echo "dfu-util did not complete the download of $2 (exit $rc)" >&2; exit 1; }
}
if [ "$FROM" -le 1 ]; then step "1/6 tiboot3.bin -> ROM (DFU alt bootloader)";            dfu_dl bootloader "$LOADER/tiboot3.bin"; fi
if [ "$FROM" -le 2 ]; then step "2/6 tispl.bin -> R5 SPL (DFU alt bootloader)";             dfu_dl bootloader "$LOADER/tispl.bin"; fi
if [ "$FROM" -le 3 ]; then step "3/6 flasher u-boot.img -> A53 SPL (DFU alt u-boot.img); its bootcmd: partconf, bootbus, fastboot"; dfu_dl u-boot.img "$FLASHER/u-boot.img"; fi

step "waiting for fastboot"
i=0; until fastboot devices 2>/dev/null | grep -v "no permissions" | grep -qi fastboot; do i=$((i+1)); [ $i -lt 240 ] || { echo "no fastboot device after 120 s (is it attached to WSL? usbipd list)" >&2; exit 1; }; sleep 0.5; done
fastboot getvar max-download-size 2>&1 | head -1

if [ $ERASE = 1 ]; then step "erase mmc0"; fastboot erase mmc0; fi
if [ "$FROM" -le 4 ]; then step "4/6 boot0.img -> eMMC boot0 (fastboot flash mmc0boot0)"; fastboot flash mmc0boot0 "$BOOT0"; fi
if [ "$FROM" -le 5 ]; then step "5/6 $(basename "$SIMG") -> eMMC user area (fastboot flash mmc0, sparse)"; fastboot flash mmc0 "$SIMG"; fi
step "6/6 reboot"
fastboot reboot
step "done"
