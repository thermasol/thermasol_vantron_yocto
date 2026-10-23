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

## Fastboot flashing (fast path)

`flash-fastboot.sh` replaces the single 627 MB DFU write with a fastboot
session and cuts the eMMC write from ~107 s to a fraction of that. The
sequence:

1. `tiboot3.bin`, `tispl.bin` over DFU, as before (stock Vantron loader or
   the `vt-flasher/` copies built from our U-Boot source - the script
   defaults to the latter, `-l bin/loader` picks the stock one).
2. `vt-flasher/u-boot.img` over DFU: the `u-boot-vt-flasher` recipe, Vantron's
   `vt_sbc_am62l_dfu_defconfig` plus a fastboot gadget and HS200. It never
   goes on a unit.
   Its bootcmd runs `mmc partconf`, `mmc bootbus` and `fastboot usb 0`.
3. `fastboot flash mmc0boot0 boot0.img` - eMMC boot0 (tiboot3 + tispl +
   u-boot.img from this build, assembled by `make-boot0.py` at image build
   time at the offsets the three DFU alts used).
4. `fastboot flash mmc0 <image>.rootfs.wic.simg` - the whole user area from
   the sparse `.wic` (`wic2simg.py`, also run at image build time; zero
   blocks are DONT_CARE chunks and are neither sent nor written).
5. `fastboot reboot`.

Measured over WSL/usbipd (which adds several seconds to each of the three
re-enumerations): 48 s end to end with the uEnv stage still present, against
2 min 15 s for the DFU station; the eMMC user area took 18 s of that.

Why it is faster: U-Boot's DFU moves data in 4 KB control transfers (~6 MB/s
measured); fastboot uses bulk transfers, and the sparse image is about half
the raw size. `-e` runs `fastboot erase mmc0` first for a zeroed device.

Needs `dfu-util` and `fastboot` (android-sdk-platform-tools) on the host and
USB access. The Windows Flash Station does the same thing with `fastboot.exe`;
see `station/`.

`stage-station.sh -s <station folder> -v <version>` copies a build's files onto the Windows Flash Station
(loader, boot0, sparse and raw images), sets `version.txt` and rewrites the product's manifest; it writes
into the production station folder, so run it deliberately. `station/` holds the station's own
`FlashStation.ps1` (v1.2.0, fastboot steps), its config sample and README under version control.

Both helper scripts live in the layer (`layers/meta-vantron/meta-custom/scripts/`)
because the image build runs them; the copies here are symlinks.
