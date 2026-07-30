# flash-tools

Flashes the vt-sbc-am62l-yocto build onto an AM62L board over USB DFU. This is
Vantron's own DFU flashing tool (`dfu_flash.py` + `src/`, copied as-is from
their reference package) pointed at **our** build output instead of theirs —
`bin/loader/` and `bin/images/` are symlinks into
`build/deploy-ti/images/vt-sbc-am62l/`, so they always follow the latest
`bitbake vtlinux-image-qt5-minimal` build without needing to be re-copied.

## Usage

1. Put the board in USB DFU loader mode and connect it.
2. If using WSL2, attach the device (check `usbipd list` for the busid):
   ```
   usbipd.exe attach --wsl --busid <busid>
   ```
3. From this directory:
   ```
   ./flash.sh
   ```

`flash.sh` requires `sudo` (matching Vantron's own tool) since raw USB access
to the DFU device needs root. It flashes, in order: `tiboot3.bin`, `tispl.bin`,
`u-boot.img` (the R5/A53 bootloader chain, to Boot0), then the full
`emmc.wic` disk image (bootloader env + rootfs partitions) to the eMMC user
data area. Pass `-r` to have it reset the board into the newly-flashed image
afterward: `./flash.sh -r`.

## Updating after a new build

Nothing to do — `bin/loader/*` and `bin/images/*` are symlinks to the stable
(non-timestamped) filenames Yocto produces, so a fresh `bitbake
vtlinux-image-qt5-minimal` is picked up automatically on the next `./flash.sh`.
