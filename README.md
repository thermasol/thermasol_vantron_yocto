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

Both files are portable (paths are relative to `${TOPDIR}`), so keep them that way: don't commit absolute
paths or host-specific tuning into `local.conf`. For per-machine settings, create `build/conf/site.conf`
(gitignored) — BitBake reads it automatically before `local.conf`. Note that `local.conf` uses `?=` for
`DL_DIR` / `SSTATE_DIR`, so a plain `=` in `site.conf` overrides them, e.g. to point at a shared cache:

```
DL_DIR = "/mnt/yocto-cache/downloads"
SSTATE_DIR = "/mnt/yocto-cache/sstate-cache"
BB_NUMBER_THREADS = "16"
PARALLEL_MAKE = "-j 16"
```

### Host workaround wrappers (`build/scripts/`)

Checked in alongside `build/conf/`, and wired up from `local.conf` via `${TOPDIR}`, so a fresh clone
needs no setup:

| File | Wired via | Works around |
| --- | --- | --- |
| `dpkg-deb-retry-v2.sh` | `DPKG_BUILDCMD` | `dpkg-deb`'s internal tar hitting the pseudo `*at()` race, and its silent truncated-`.deb` bug. Retries, validates the archive has both `control.tar` and `data.tar`, then falls back to `pydeb-build.py`. |
| `pydeb-build.py` | called by the above | Pure-Python `.deb` builder used when `dpkg-deb` can't produce a valid archive. |
| `tar` | `PATH:prepend` | GNU tar >= 1.35 emitting PAX `x` headers that this host's `dpkg` can't unpack during `do_rootfs`. Appends `--format=gnu`, leaving callers that ask for `--format`/`--posix`/`--xattrs`/`-H` alone. |

Each script carries a full comment header explaining the failure mode it addresses. They are host-side
Ubuntu 24.04 / WSL2 workarounds, not product configuration — drop them if a fixed `pseudo`/`dpkg` lands.

## Building the SignaTouch IR image

```bash
cd ~/thermasol_vantron_yocto
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

See `flash-tools/` for how to write the `.wic` to the board. Two more files land next to it for the fastboot
flash path (`flash-tools/README.md`): `...rootfs.wic.simg`, the same image as an Android sparse file, and
`...boot0.img`, this build's tiboot3/tispl/u-boot.img laid out for the eMMC boot0 partition. The flasher
U-Boot they are written with is a separate build, `bitbake u-boot-vt-flasher`, deployed under `vt-flasher/`.

## Building the SignaTouch image

```bash
cd ~/thermasol_vantron_yocto
source layers/poky/oe-init-build-env build
bitbake vtlinux-image-signatouch
```

The two products share this BSP and differ only in the application on top of it:

| | SignaTouch IR | SignaTouch |
|---|---|---|
| Image | `vtlinux-image-signatouch-ir` | `vtlinux-image-signatouch` |
| App recipe | `signatouch-ir` | `signatouch` |
| App repo / branch | `SignaTouch_IR.git` @ `olivia_yocto` | `SignaSteam.git` @ `old_ui_Vantron` |
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
`QT_QPA_EGLFS_ROTATION` into `/etc/qt5/rotation.env`, and sets the panel backlight to the product level (100 of 255; the kernel DT
`default-brightness-level`, this script and the app all use 100, and `systemd-backlight@` is masked so a saved
value is never restored over it).
`signatouch.service` sources `rotation.env` at unit start, which is why changing the orientation in the app
reboots the device.

The boot logo and splash follow the same setting. The script also writes `optargs_rotation=fbcon=rotate:3`
into the rootfs's `/boot/image.env` (read by U-Boot from the rootfs partition, the same place the kernel
Image is loaded from, and appended to the kernel command line; fbcon rotates its boot logo with the console,
`CONFIG_FRAMEBUFFER_CONSOLE_ROTATION`) and `PSPLASH_ARGS=-a 90` into `/etc/psplash.env` (sourced by
`psplash-start.service`). Note that at runtime `mount-partitions.service` has the vfat boot partition mounted
over `/boot`, hiding that directory; the script reaches it through a bind mount of `/`, since a plain write
to `/boot/image.env` lands on the vfat where U-Boot never looks. Both are counter-clockwise quarter turns, the same direction as Qt's
rotation 90, so on a landscape unit the ThermaSol mark reads the right way up from the first frame. On
portrait units both are empty. The IR image's own line in `image.env` (its boot logo selection) is preserved.

#### Qt eglfs and successive top-level windows

The first-boot flow is three top-level windows in a row (orientation dialog, setup wizard, MainWindow).
Stock Qt 5.15 eglfs deletes the raster compositing context when the root window is destroyed but leaves
`qt_gl_global_share_context()` pointing at it, so the next window shares with freed memory and the app
segfaults in `QEglFSIntegration::createPlatformOpenGLContext` (seen as a crash on the wizard's Finish on
landscape units; portrait only survived by heap-reuse luck). `recipes-qt/qt5/qtbase_%.bbappend` backports
Qt's own fix (Qt 6 commit `f3db9abebf7e`, never applied to 5.15).

The second problem in the same flow is in the `eglfs_kms` backend: the KMS mode, and with it the primary
plane's framebuffer, is set once per screen. When the window that owns the screen's `gbm_surface` is
destroyed its framebuffers are removed and the kernel unbinds the plane (CRTC active, `fb=0` in
`/sys/kernel/debug/dri/0/state`). The next window's page flips all fail with `EBUSY` ("Could not queue DRM
page flip on screen DSI1 (Device or resource busy)" in the journal) and the panel stays black while the app
runs normally. The same bbappend backports Qt's QTBUG-122663 fix (Qt 6.5 commits `818f8de64a2b` and
`0a3b5042a566`), which re-runs `drmModeSetCrtc` for the next window's first frame.

### Room temperature sensor

The app reads a thermistor divider on ADC channel 0 via
`/sys/bus/iio/devices/iio:device0/in_voltage0_raw`. The pieces that make that node exist:

- `tscadc0` is `status = "okay"` with `ti,adc-channels = <0 1>` in `vt-sbc-am62l.dtsi` (kernel repo).
- `CONFIG_MFD_TI_AM335X_TSCADC=m`, `CONFIG_TI_AM335X_ADC=m`, `CONFIG_IIO_KFIFO_BUF=m` in
  `vt_sbc_am62l_defconfig`, with `CONFIG_IIO`/`CONFIG_IIO_BUFFER` built in.
- The matching `kernel-module-*` packages, named explicitly in `SENSOR_PACKAGES` in the image recipe.
- `/etc/modules-load.d/ti-adc.conf`, shipped by the `signatouch` recipe, so the modules load at boot rather
  than depending on udev coldplug timing.

The sensor is a single-ended epoxy NTC, 100 kohm at 25 C, B = 3950 K, on the low side of a divider with
the board's 100 kohm pull-up to the 1.8 V ADC reference. The app (`EquipmentCanInterface`) solves the
divider for the thermistor resistance and applies the beta equation, so 25 C reads at mid-scale (raw ~2048),
10 C near 2740 and 40 C near 1420. The device tree gives the ADC a long sample delay and 16x averaging on
that channel (`vt-sbc-am62l-adc-step-delays.patch`) because the divider's ~50 kohm source impedance is far
above what the sample-and-hold settles on with the driver defaults.

Sanity check on target: `cat /sys/bus/iio/devices/iio:device0/in_voltage0_raw` should return a 0-4095 count,
or run `iio_info` (from `libiio-tests`, built local-backend only via our `libiio_%.bbappend`) to see both
channels with their scale. Channel 0 has an on-board pull-up to 1.8 V, so with nothing connected it reads
about 4093 (Vantron's ADC bring-up doc shows the same); channel 1 floats near 60.

### Output

```
build/deploy-ti/images/vt-sbc-am62l/vtlinux-image-signatouch-vtlinux-vt-sbc-am62l.rootfs.wic
```

Same layout and flashing procedure as the IR image above.

### Service access on a shipped unit

Production images are built without `debug-tweaks`: root has a password,
there is no SSH server (the board has no network interface in this
product), U-Boot does not accept a key to interrupt autoboot
(`CONFIG_BOOTDELAY=-2`), and adbd is installed but not started at boot.

**Serial console (UART).** UART0 at 115200 8N1 on the SBC's onboard
CP2102N micro-USB bridge (not the pin header, that one is RS-485). Log in
as `root`, password `steamy17%`. The SHA-512 hash of it is
`KIOSK_ROOT_PASSWORD_HASH` in `vtlinux-image-kiosk.inc` (generate a
replacement with `openssl passwd -6`). The same password applies to the
IR image.

**ADB.** In the SignaTouch app go to Settings > Equipment > Diagnostics
and tap the "Diagnostics" title at the top of the screen five times, each
tap within two seconds of the last. Nothing is shown on screen. The app
creates `/etc/usb-debugging-enabled` and starts
`android-tools-adbd.service`; the marker persists, so ADB stays on for
that unit across reboots. `adb shell` then gives a root shell over the
USB gadget (on WSL the gadget, USB ID 18d1:d002, has to be attached with
usbipd first). To turn it back off:
`rm /etc/usb-debugging-enabled && systemctl stop android-tools-adbd`.
The IR image keeps ADB on at boot.

**Bench builds.** Put any of these in the gitignored `build/conf/site.conf`:

```
EXTRA_IMAGE_FEATURES = "debug-tweaks"                  # passwordless root, root login on every console
KIOSK_ADB_AT_BOOT:pn-vtlinux-image-signatouch = "1"    # adbd up at boot without the gesture
UBOOT_BOOTDELAY = "3"                                  # U-Boot prompt back
```

## Updating the boot splash (psplash) logo

Each product has its own splash. psplash compiles the PNG into the executable, and the base recipe
(`layers/poky/meta/recipes-core/psplash/psplash_git.bb`) builds one binary per entry in `SPLASH_IMAGES`,
packaged as `psplash-<outsuffix>` and registered as an update-alternatives provider of `/usr/bin/psplash`.
The wiring lives in `layers/meta-vantron/meta-custom/recipes-core/psplash/psplash_git.bbappend`:

| PNG (in `recipes-core/psplash/files/`) | Package | Used by |
|---|---|---|
| `signatouch.png` (ThermaSol wordmark, same image as the kernel boot logo) | `psplash-signatouch` | `signatouch` recipe -> `vtlinux-image-signatouch` |
| `signatouch-ir.png` | `psplash-signatouch-ir` | `signatouch-ir` recipe -> `vtlinux-image-signatouch-ir` |
| `yocto.png` | `psplash-default` | everything else (core, weston, qt5-minimal, factory) |

Artwork is tied to a product by the `RDEPENDS` in that product's **app** recipe (`psplash-signatouch` /
`psplash-signatouch-ir`), not by the image recipe -- so an image gets the right splash purely by installing
its app. `psplash-default` is always pulled in alongside (psplash's own `RDEPENDS` chain), so the product
packages carry `ALTERNATIVE_PRIORITY 200` against the default's 100; without that the tie would be resolved
arbitrarily and the splash would be a coin flip.

`signatouch-ir.png` is the Almost Heaven Saunas logo that previously shipped as `yocto.png` (and, because
`SPLASH_IMAGES` was a single global entry, went into every image built from this layer).
**`signatouch.png` is currently still the Yocto placeholder** -- the real artwork has not been added yet.

To replace a product's logo:

1. Overwrite the matching PNG in `layers/meta-vantron/meta-custom/recipes-core/psplash/files/`, keeping the
   filename. Match the panel's resolution/aspect ratio (480x854) for a clean result -- psplash scales to the
   framebuffer, but starting from the right aspect avoids distortion.
2. Rebuild:
   ```bash
   bitbake -c cleansstate psplash && bitbake psplash
   # or rebuild the full image; psplash is pulled in as a dependency
   ```

To add a splash for a *new* product: add a `file://<name>.png;outsuffix=<name>` entry to `SPLASH_IMAGES`, an
`ALTERNATIVE_PRIORITY_psplash-<name> = "200"` line beside it, and `psplash-<name>` to that product's app
recipe `RDEPENDS`.

No manual `make-image-header.sh` step is needed -- the base recipe's `do_compile()` runs it against each PNG
and rebuilds the binary.
