# vt-sbc-am62l-yocto

Yocto/OpenEmbedded BSP for Vantron's **VT-SBC-AM62L** (TI AM62L SoC), Yocto "scarthgap" release.

Flattened snapshot of Vantron's `repo`-managed workspace (manifest `scarthgap-VT-SBC-AM62L-v1.0.7.xml`), pulled from
`git.vantrontech.us` via:

```bash
repo init -u ssh://git@git.vantrontech.us:8222/linux/ti/6.12/standard/VT-SBC-AM62L/manifests-yocto.git \
  -b scarthgap -m scarthgap-VT-SBC-AM62L-v1.0.7.xml \
  --repo-url ssh://git@git.vantrontech.us:8222/linux/git-repo.git
repo sync
```

Each layer's nested `.git` history has been stripped and everything committed here as one flat tree, since
`git.vantrontech.us` access isn't guaranteed to persist.

## Layers (manifest v1.0.7 pinned revisions)

| Layer | Revision |
|---|---|
| poky | `200d12b6a58ad961d60a7774ca0f7a9d29498724` |
| meta-arm | `950a4afce46a359def2958bd9ae33fc08ff9bb0d` |
| meta-clang | `eea5ec23155cfd47781599fdc9a92c21e4caffc8` |
| meta-lts-mixins | `a8046d5ec53b1856169ac795aa87cb0d5db84c04` |
| meta-openembedded | `72018ca1b1a471226917e8246e8bbf9a374ccf97` |
| meta-qt6 | `586a6cb5aec755803a3be3cec359baafe89d6432` |
| meta-ti | `59354be6c2362b9f7d0a3cd2c017f2f3c6ab7f2e` |
| meta-vantron/meta-custom | `ad854292641b5f4804ae74408e2e8dbc0c4e5177` |
| meta-vantron/meta-vtlinux-base | `1dde6939e4971428f41ca07ab8ed3b5f3cae4ea6` |

## Known deviation from Vantron upstream

`layers/meta-vantron/meta-vtlinux-base/recipes-samples/eeprom-util/eeprom-util_3.2.0.bb` originally fetched from
`git.vantrontech.us:8222/linux/vantron/applications/eeprom-utils.git`, which this GitHub account has no read
access to. The recipe's own `HOMEPAGE` and the already-vendored `Allow-cross-compilation.patch` (a 3-line Makefile
tweak) show Vantron's fork is just upstream CompuLab's `eeprom-util` v3.2.0 — confirmed by an exact `LICENSE`
checksum match. `SRC_URI`/`SRCREV` were repointed at the public
[`compulab/eeprom-util`](https://github.com/compulab/eeprom-util) repo, tag `v3.2.0`
(`257db099e8339404510eded3521da5370b321041`), instead.

## Build

```bash
source layers/poky/oe-init-build-env build
bitbake <image-name>
```

`build/conf/local.conf` and `build/conf/bblayers.conf` are already checked in and configured for
`MACHINE = "vt-sbc-am62l"` / `DISTRO = "vtlinux"`, so `oe-init-build-env` just reuses them — no manual
config is needed on a fresh clone.

## Building the SignaTouch IR image

```bash
cd ~/vt-sbc-am62l-yocto
source layers/poky/oe-init-build-env build
bitbake vtlinux-image-signatouch-ir
```

This pulls in the `signatouch-ir` recipe (`layers/meta-vantron/meta-custom/recipes-applications/signatouch-ir/`),
which fetches the app from `git@github.com:thermasol/SignaTouch_IR.git` (currently pinned to the `olivia_yocto`
branch via `SRCREV` in the recipe) plus `ThermaCan` (CAN library at `master`, shared with the `signatouch`
recipe, built from source for the target rather than using the prebuilt libs vendored in the app repo, which
are wrong-architecture) and the separate
`arcsliderplugin` recipe (custom Qt slider widget, also built from source). All three need SSH access to the
`thermasol` GitHub org to fetch.

A full from-scratch build takes several hours; an incremental rebuild (e.g. after bumping `SRCREV` or editing a
recipe) reuses `build/sstate-cache` and only rebuilds what changed — typically a few minutes.

### Output

```
build/deploy-ti/images/vt-sbc-am62l/vtlinux-image-signatouch-ir-vtlinux-vt-sbc-am62l.rootfs.wic
```

This is a symlink to the timestamped file from the most recent build
(`...rootfs-<YYYYMMDDHHMMSS>.wic`). It's a single GPT-partitioned disk image (boot + rootfs + userfs) built
against `mmcblk0` — the same image is used for both SD card and onboard eMMC on this board, there's no
separate "SD" vs "eMMC" variant. Other formats (`.ext4`, `.ext4.gz`, `.tar.xz`, `.manifest`, SPDX SBOMs) land
alongside it in the same directory.

See `flash-tools/` for how to write the `.wic` to the board.

## Building the SignaTouch image

```bash
cd ~/vt-sbc-am62l-yocto
source layers/poky/oe-init-build-env build
bitbake vtlinux-image-signatouch
```

The two products share this BSP and differ only in the application on top of it:

| | SignaTouch IR | SignaTouch |
|---|---|---|
| Image | `vtlinux-image-signatouch-ir` | `vtlinux-image-signatouch` |
| App recipe | `signatouch-ir` | `signatouch` |
| App repo / branch | `SignaTouch_IR.git` @ `olivia_yocto` | `SignaSteam.git` @ `signa_olivia` |
| Binary | `/usr/bin/SignaTouch_IR` | `/usr/bin/SignaSteam` |
| Extra Qt | `qtsvg` + `arcsliderplugin` | — (PNG/TTF resources only) |
| Screen rotation | fixed landscape | user-selectable, persisted |
| Room temp sensor | no | yes (TSCADC via IIO) |

The `signatouch` recipe
(`layers/meta-vantron/meta-custom/recipes-applications/signatouch/`) fetches the app from
`git@github.com:thermasol/SignaSteam.git` (the upstream repo still carries the SignaSteam name) plus
`ThermaCan`, which it builds from source for the target rather than using the wrong-architecture prebuilt
libs vendored in the app repo. Both repos need SSH access to the `thermasol` GitHub org to fetch.

Both product recipes pin the same `ThermaCan` revision — `master` — so there is one CAN library across the
two images. Master is required by `signatouch`, whose app calls the `lctesv_slave_*` API, and it is a clean
superset for `signatouch-ir`: every `extern` in that app's vendored `include/thermacan.h` is byte-identical
to master's except `psb_get_psp_maintenance_counter()`, which master renamed and which `SignaTouch_IR` never
calls. (The legacy `signasteam` recipe is unaffected — it builds ThermaCan from a vendored source copy, not
from git.)

`signatouch` conflicts with the older `signasteam` recipe — both install `/usr/bin/SignaSteam` and
`/usr/lib/libthermacan.so`. `signasteam` builds the same app from a vendored source copy; `signatouch`
builds it from git and is the one to use.

### Screen rotation

The app stores the user's choice as `orientationSelected` in `/opt/ThermaSol/SignaSteam.conf` (first-boot
wizard, or Settings > Device > Orientation). At each boot `set-display-rotation.sh` (from the
`display-rotation` recipe) reads it, writes `/etc/qt5/eglfs_kms.json` and
`QT_QPA_EGLFS_ROTATION` into `/etc/qt5/rotation.env`, and forces the panel backlight to full brightness.
`signatouch.service` sources `rotation.env` at unit start, which is why changing the orientation in the app
reboots the device.

### Room temperature sensor

The app reads a thermistor divider on ADC channel 0 via
`/sys/bus/iio/devices/iio:device0/in_voltage0_raw`. The pieces that make that node exist:

- `tscadc0` is `status = "okay"` with `ti,adc-channels = <0 1>` in `vt-sbc-am62l.dtsi` (kernel repo).
- `CONFIG_MFD_TI_AM335X_TSCADC=m`, `CONFIG_TI_AM335X_ADC=m`, `CONFIG_IIO_KFIFO_BUF=m` in
  `vt_sbc_am62l_defconfig`, with `CONFIG_IIO`/`CONFIG_IIO_BUFFER` built in.
- The matching `kernel-module-*` packages, named explicitly in `SENSOR_PACKAGES` in the image recipe.
- `/etc/modules-load.d/ti-adc.conf`, shipped by the `signatouch` recipe, so the modules load at boot rather
  than depending on udev coldplug timing.

Sanity check on target: `cat /sys/bus/iio/devices/iio:device0/in_voltage0_raw` should return a 0-4095 count.

### Output

```
build/deploy-ti/images/vt-sbc-am62l/vtlinux-image-signatouch-vtlinux-vt-sbc-am62l.rootfs.wic
```

Same layout and flashing procedure as the IR image above.

## Updating the boot splash (psplash) logo

The splash image is set via `SPLASH_IMAGES` in
`layers/meta-vantron/meta-custom/recipes-core/psplash/psplash_git.bbappend`, currently pointing at
`layers/meta-vantron/meta-custom/recipes-core/psplash/files/yocto.png` (still the placeholder image, not a
real logo).

The base recipe (`layers/poky/meta/recipes-core/psplash/psplash_git.bb`) already automates the old manual
"run `make-image-header.sh`, rename to `psplash-poky-img.h`" process — its `do_compile()` fetches psplash's
own source, runs that script against whatever PNG `SPLASH_IMAGES` points at, and rebuilds. No manual step is
needed.

To replace the logo:

1. Drop your PNG into `layers/meta-vantron/meta-custom/recipes-core/psplash/files/` — either overwrite
   `yocto.png` in place, or add a new file and update the `SPLASH_IMAGES` path in the `.bbappend` to match.
   Match the panel's resolution/aspect ratio (480x854) for a clean result — psplash scales to the framebuffer,
   but starting from the right aspect avoids distortion.
2. Rebuild:
   ```bash
   bitbake -c cleansstate psplash && bitbake psplash
   # or rebuild the full image; psplash is pulled in as a dependency
   ```
