# Shared "single-app kiosk" slimming policy for the product images
# (vtlinux-image-signatouch-ir, vtlinux-image-qt5-minimal).
#
# These boards run exactly one application on the local display and nothing
# else. The stock image inherits core-image, which starts from
# packagegroup-base-extended, and the vendor's packagegroup-framework-base,
# which between them drag in a general-purpose desktop/server userland:
# Bluetooth OBEX -> libical -> ICU (34 MB), bluez5-testtools -> the whole
# python3 runtime (34 MB), ofono (3G), avahi (zeroconf), rpcbind/nfs-utils,
# neard (NFC), nss/nspr, apt+dpkg+perl+gnupg, gdb, and 1467 kernel modules
# for hardware this SoC does not have (nouveau, mlx5, iwlwifi, tegra audio,
# Raspberry Pi vc4, LSI SAS...).
#
# Measured baseline before this file: 2040 packages, 634 MB of package
# content, 934 MB ext4, 1.34 GB .wic.
#
# Everything here is deliberately package-level (IMAGE_INSTALL /
# IMAGE_FEATURES / BAD_RECOMMENDATIONS) rather than DISTRO_FEATURES-level.
# Dropping "bluetooth"/"wifi"/"3g"/"nfc"/"zeroconf"/"nfs" from
# DISTRO_FEATURES would have the same rootfs effect but re-triggers a
# world rebuild; the trimming below only re-runs do_rootfs, so it can be
# built, flashed and measured in minutes. See "Phase 2" at the bottom for
# the changes that do need a rebuild.

# ---------------------------------------------------------------------------
# Knobs. Each defaults to the stripped-down production answer; set to "1"
# in the image recipe (or local.conf) to put the feature back.
# ---------------------------------------------------------------------------

# Wi-Fi (AP6256 via bcmdhd) - wpa-supplicant, iw, regdb, firmware, and the
# cfg80211/mac80211/bcmdhd modules. ~12 MB.
KIOSK_WIFI ?= "0"

# Bluetooth (AP6256 via H4 UART) - bluez5 + brcm-patchram-plus. ~7 MB on its
# own, but see KIOSK_BLUETOOTH_TOOLS: it is the bluez *tools* packages, not
# bluez5 itself, that cost 68 MB.
KIOSK_BLUETOOTH ?= "0"

# bluez5-testtools / bluez5-noinst-tools / bluez-tools /
# packagegroup-tools-bluetooth. Only meaningful with KIOSK_BLUETOOTH = "1".
# These are what pull python3 (34 MB) and, via bluez5-obex -> libical, ICU
# (34 MB). Keep this "0" even on units that need working Bluetooth.
KIOSK_BLUETOOTH_TOOLS ?= "0"

# gdb/gdbserver/strace/perf (packagegroup-core-tools-debug), evtest,
# kmscube, libdrm-tests, qtbase-tools, fontconfig-utils, iperf3, tcpdump,
# memtester, minicom, usbutils, libiio-tests. ~60 MB.
KIOSK_DEBUG_TOOLS ?= "0"

# adbd over the USB gadget. Separate from KIOSK_DEBUG_TOOLS and defaulted ON
# because this is the service access path to a unit with no keyboard and no
# getty on the display - "adb shell" is how the board gets looked at in the
# field. ~1.5 MB for adbd + the gadget config + the usb_f_fs/libcomposite
# modules. Set to "0" for a locked-down production build.
KIOSK_ADB ?= "1"

# apt/dpkg on target (IMAGE_FEATURES "package-management"). Costs ~25 MB
# (apt, dpkg, perl, gnupg, apt-conf) and adds run-postinsts.service to the
# first boot. Images are field-updated by reflashing, not by apt.
KIOSK_PKG_MANAGEMENT ?= "0"

# Escape hatch: extra kernel modules to install, as full package names
# (e.g. "kernel-module-foo"). The allowlist in section 2 came from `lsmod`
# on a working unit, which is the ground truth - anything a new unit or a
# new defconfig loads and that isn't listed there belongs here.
KIOSK_EXTRA_MODULES ?= ""

# ---------------------------------------------------------------------------
# 1. Don't start from packagegroup-base-extended (-100 MB, and drops
#    ofono/avahi/rpcbind/neard from the boot).
#
#    core-image.bbclass seeds IMAGE_INSTALL with
#    "packagegroup-core-boot packagegroup-base-extended", and
#    packagegroup-base-extended is packagegroup-base + the 3g, bluetooth,
#    nfc and wifi groups. Of everything under it this image actually needs
#    kmod (to load the panel modules), the ext2 tools (resize-helper runs
#    e2fsck/resize2fs on first boot) and the vfat tools (fsck of the boot
#    partition) - so pull those three in by name and leave the rest out.
# ---------------------------------------------------------------------------
IMAGE_INSTALL:remove = "packagegroup-base-extended"

KIOSK_BASE_INSTALL = " \
    kmod \
    e2fsprogs-e2fsck \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    dosfstools \
    ${@bb.utils.contains('COMBINED_FEATURES', 'alsa', 'alsa-state alsa-utils-amixer alsa-utils-aplay', '', d)} \
"

# ---------------------------------------------------------------------------
# 2. Kernel modules: an allowlist instead of the kernel-modules meta-package
#    (-62 MB).
#
#    kernel-modules RDEPENDS on every one of the 1467 modules the TI
#    multi-platform defconfig builds, 66.8 MB of them. Everything on this
#    board's critical path is built into the kernel already - verified in
#    build/tmp-default/work-shared/vt-sbc-am62l/kernel-build-artifacts/.config:
#
#      CAN      CONFIG_CAN / CAN_DEV / CAN_RAW / CAN_BCM /
#               CAN_M_CAN_PLATFORM=y, PHY_CAN_TRANSCEIVER=y (ti,tcan1042)
#      Display  CONFIG_DRM=y, DRM_TIDSS=y, DRM_MIPI_DSI=y,
#               DRM_FBDEV_EMULATION=y, FB=y
#      Touch    CONFIG_TOUCHSCREEN_CHSC5XXX=y (chipsemi,chsc_cap_touch),
#               INPUT_EVDEV=y
#      Backlight CONFIG_PWM=y, PWM_TIECAP=y (the ecap0 provider)
#      Storage  MTD_SPI_NOR=y, EEPROM_AT24=y
#      PMIC     MFD_TPS65219=y, REGULATOR_TPS65219=y (ti,tps65214)
#      Misc     KEYBOARD_GPIO=y, LEDS_GPIO=y
#
#    The list below is every module `lsmod` reports on a working unit, minus
#    the Wi-Fi/Bluetooth ones, and it replaces an earlier guess derived only
#    from the DTS compatibles. Deriving it from the DTS was not enough:
#    cdns_dsi and cdns_dphy bind to each other through a DT phandle rather
#    than through an exported symbol, so kernel-module-split generates no
#    RDEPENDS between them, and an allowlist that named only cdns-dsi would
#    have shipped a board with a dead panel. `lsmod` on target is the
#    authority; re-check it after any defconfig change.
#
#    Only the leaves are named. kernel-module-split reads modinfo and emits
#    RDEPENDS for symbol dependencies, so these come in automatically:
#      panel-sitronix-st7701 -> drm-mipi-dbi
#      pwm-bl                -> backlight
#      ti-am335x-adc         -> kfifo-buf, ti-am335x-tscadc
#      usb-f-fs              -> libcomposite
#      dthev2                -> md5
#      8021q                 -> garp, mrp -> stp -> llc
#      ip-tables             -> x-tables
#
#    All 18 of them together are 651 KiB, against the 62 MB that dropping
#    kernel-modules saves - so where there was any doubt the module stayed
#    in. See the note on the last group.
# ---------------------------------------------------------------------------
IMAGE_INSTALL:remove = "kernel-modules"

# Display: DSI host + D-PHY + panel + backlight. Losing any one of these is
# a black screen.
KIOSK_MODULES_DISPLAY = " \
    kernel-module-cdns-dsi \
    kernel-module-cdns-dphy \
    kernel-module-panel-sitronix-st7701 \
    kernel-module-pwm-bl \
"

# Sensors: the AM335x-compatible ADC (ti_am335x_adc on the tscadc MFD) and
# the K3 bandgap temperature sensor that backs the SoC thermal zones - i.e.
# thermal throttling and shutdown protection. Neither appears anywhere in
# vt-sbc-am62l.dtsi's compatible strings, which is the other reason the
# DTS-only allowlist was wrong.
KIOSK_MODULES_SENSORS = " \
    kernel-module-ti-am335x-adc \
    kernel-module-k3-j72xx-bandgap \
"

# Crypto offload: the TI DTHE v2 engine plus the ARMv8-CE CRC-T10DIF
# implementation. Both load off the DT/CPU features on their own.
KIOSK_MODULES_CRYPTO = " \
    kernel-module-dthev2 \
    kernel-module-crct10dif-ce \
"

# Networking that is loaded on a working unit but shows "Used by 0" or has
# no visible consumer in these layers: VLAN, iptables and fuse. Nothing in
# meta-vantron references them, so they are most likely autoload artifacts
# of packages this image no longer installs - but libvtcom, vtvdm and
# eeprom-link are closed vendor binaries whose syscalls nobody here can
# audit, and 382 KiB is not worth finding out the hard way in the field.
# Drop them if you confirm nothing wants them. ipv6 is genuinely in use
# (lsmod: "Used by 36").
KIOSK_MODULES_NET = " \
    kernel-module-ipv6 \
    kernel-module-8021q \
    kernel-module-ip-tables \
    kernel-module-fuse \
"

KIOSK_MODULES = " \
    ${KIOSK_MODULES_DISPLAY} \
    ${KIOSK_MODULES_SENSORS} \
    ${KIOSK_MODULES_CRYPTO} \
    ${KIOSK_MODULES_NET} \
    ${@bb.utils.contains('KIOSK_ADB', '1', 'kernel-module-usb-f-fs', '', d)} \
    ${@bb.utils.contains('KIOSK_WIFI', '1', 'kernel-module-bcmdhd kernel-module-cfg80211 kernel-module-mac80211', '', d)} \
    ${@bb.utils.contains('KIOSK_BLUETOOTH', '1', 'kernel-module-hci-uart', '', d)} \
    ${KIOSK_EXTRA_MODULES} \
"

# ---------------------------------------------------------------------------
# 3. Optional feature blocks
# ---------------------------------------------------------------------------
KIOSK_WIFI_INSTALL = "${@bb.utils.contains('KIOSK_WIFI', '1', \
    'wpa-supplicant iw wireless-regdb-static linux-firmware-ap6256', '', d)}"

KIOSK_BT_INSTALL = "${@bb.utils.contains('KIOSK_BLUETOOTH', '1', \
    'bluez5 brcm-patchram-plus linux-firmware-ap6256', '', d)} \
    ${@bb.utils.contains('KIOSK_BLUETOOTH_TOOLS', '1', \
    'packagegroup-tools-bluetooth bluez-tools', '', d)}"

KIOSK_DEBUG_INSTALL = "${@bb.utils.contains('KIOSK_DEBUG_TOOLS', '1', \
    'packagegroup-core-tools-debug evtest kmscube libdrm-tests qtbase-tools \
     fontconfig-utils iperf3 tcpdump memtester minicom usbutils libiio-tests \
     i2c-tools mmc-utils libgpiod-tools parted gptfdisk', '', d)}"

# adbd needs both the daemon and android-tools-conf-ti, which is what writes
# the USB gadget configfs setup the usb_f_fs/libcomposite modules back.
KIOSK_ADB_INSTALL = "${@bb.utils.contains('KIOSK_ADB', '1', \
    'android-tools-adbd android-tools-conf-ti', '', d)}"

IMAGE_FEATURES += "${@bb.utils.contains('KIOSK_PKG_MANAGEMENT', '1', 'package-management', '', d)}"

CORE_IMAGE_EXTRA_INSTALL += " \
    ${KIOSK_BASE_INSTALL} \
    ${KIOSK_MODULES} \
    ${KIOSK_WIFI_INSTALL} \
    ${KIOSK_BT_INSTALL} \
    ${KIOSK_DEBUG_INSTALL} \
    ${KIOSK_ADB_INSTALL} \
"

# ---------------------------------------------------------------------------
# 4. Recommendations we don't want pulled in
#
#    Note for anyone extending this: BAD_RECOMMENDATIONS does NOT work here.
#    PACKAGE_CLASSES is package_deb, and rootfs_deb.bbclass only prints
#    "Debian package install does not support BAD_RECOMMENDATIONS" - apt
#    installs the Recommends: regardless. PACKAGE_EXCLUDE is no help either;
#    image.bbclass only strips names out of PACKAGE_INSTALL, so anything
#    reached as a dependency or recommendation comes back.
#
#    So RRECOMMENDS has to be cut at the recipe that declares it:
#      udev-hwdb (8.2 MB)  -> RRECOMMENDS:${PN}:remove in
#                             recipes-core/systemd/systemd_%.bbappend
#      kernel fitImage     -> RRECOMMENDS:kernel-base = "" in
#         (16 MB)            recipes-kernel/linux/linux-ti-staging_%.bbappend
#
#    shared-mime-info was a glib recommendation and left with the packages
#    that pulled glib's consumers.
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# 5. Locales (-3 MB)
#
#    vtlinux-base.inc sets IMAGE_LINGUAS = "en-us en-gb", which installs two
#    2.9 MB glibc-binary-localedata packages. The app is single-language;
#    one C.UTF-8-plus-en_US locale is enough.
# ---------------------------------------------------------------------------
IMAGE_LINGUAS = "en-us"

# ---------------------------------------------------------------------------
# 6. Filesystem sizing
#
#    IMAGE_OVERHEAD_FACTOR 1.3 (poky default) sized the old rootfs at 934 MB
#    for 634 MB of content - 300 MB of empty ext4 that has to be created,
#    written into the .wic, and pushed over DFU on every flash. The app
#    writes its state to the separate userfs partition, so the rootfs needs
#    only enough slack for logs and journald. resize-helper.service grows
#    the partition to fill the eMMC on first boot regardless.
# ---------------------------------------------------------------------------
IMAGE_OVERHEAD_FACTOR = "1.1"
IMAGE_ROOTFS_EXTRA_SPACE = "24576"

# ---------------------------------------------------------------------------
# 7. Boot-time service trimming
#
#    Measured from the stock image's
#    etc/systemd/system/*.target.wants: the app was competing at boot with
#    avahi-daemon, ofono, rpcbind, neard, iiod, firmwared, atd, crond,
#    busybox-syslog/klogd, systemd-timesyncd, systemd-resolved,
#    run-postinsts and triggerhappy. Most of those leave with their
#    packages (sections 1-3 above); these are the ones that ship inside
#    packages we still want, so mask the units.
#
#    systemd-networkd-wait-online is the important one: it is pulled in by
#    network-online.target (via remote-fs.target) and blocks for up to
#    DefaultTimeoutStartSec waiting for a carrier. On a unit with nothing
#    plugged into the Ethernet jack that is a flat ~2 minute addition to
#    the time the boot is considered "finished", and any service ordered
#    after network-online.target waits it out.
#
#    systemd-timesyncd is masked rather than removed because there is no RTC
#    battery expectation here and nothing in the app depends on wall-clock
#    accuracy at startup; if that changes, unmask it - it is not on the
#    critical path either way.
# ---------------------------------------------------------------------------
KIOSK_MASKED_UNITS = " \
    systemd-networkd-wait-online.service \
    systemd-timesyncd.service \
    systemd-resolved.service \
    remote-fs.target \
    atd.service \
    crond.service \
"

ROOTFS_POSTPROCESS_COMMAND += "kiosk_mask_units; "

kiosk_mask_units () {
    install -d ${IMAGE_ROOTFS}${sysconfdir}/systemd/system
    for unit in ${KIOSK_MASKED_UNITS}; do
        ln -sf /dev/null ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/$unit
        bbnote "kiosk: masked $unit"
    done
}

# ---------------------------------------------------------------------------
# 8. Default target
#
#    graphical.target is only meaningful when a display manager is wanted;
#    it Requires multi-user.target and adds nothing but a layer of ordering
#    on top of it. The app is a plain systemd service, so run at
#    multi-user.target - see the matching WantedBy= change in
#    signatouch-ir.service.
# ---------------------------------------------------------------------------
SYSTEMD_DEFAULT_TARGET = "multi-user.target"

# ---------------------------------------------------------------------------
# Phase 2 - the remaining wins, each of which needs a rebuild and on-device
# validation, so they are deliberately NOT enabled here:
#
#  a) clang-libllvm, 99 MB - the single biggest package in the image. The
#     AM62L has no 3D GPU, so vt-am62l.inc builds Mesa with gallium-llvm
#     (llvmpipe) and every EGL/GLES call is software-rendered on the A53s.
#     qtbase's shlib scan then RDEPENDS on libegl-mesa + libgles2-mesa,
#     which RRECOMMENDS mesa-megadriver, which RDEPENDS on clang-libllvm:
#     99 MB of LLVM + 16 MB of Mesa to software-composite a Qt Widgets app
#     that renders with the raster paint engine anyway.
#
#     SignaTouch_IR is pure QWidget (ArcSlider is a QWidget subclass, no
#     QML/Quick anywhere), and the kernel has CONFIG_DRM_FBDEV_EMULATION=y,
#     so the linuxfb platform plugin is a straight swap:
#
#         PACKAGECONFIG:remove:pn-qtbase = "eglfs gles2 kms gbm"
#         PACKAGECONFIG:append:pn-qtbase = " linuxfb"
#
#     plus QT_QPA_PLATFORM=linuxfb in signatouch-ir.service, dropping
#     mesa/libegl-mesa/libgbm from the install. That is -115 MB and it
#     removes llvmpipe context creation from app startup. It needs a qtbase
#     rebuild and on-device checking of rotation (set-display-rotation.sh
#     writes eglfs_kms.json today; linuxfb rotates via
#     QT_QPA_LINUXFB_ROTATION / fbcon) and touch input coordinates.
#
#  b) The kernel itself, 48 MB - /boot/Image is what U-Boot loads
#     (bootcmd = "run args_mmc; run loadimage; ...", loadimage reads
#     ${bootdir}/${bootfile} = /boot/Image from bootpart 0:2, the ext4
#     root). 48 MB is the TI multi-platform defconfig: 1467 modules' worth
#     of drivers plus CONFIG_DEBUG_INFO/DEBUG_KERNEL/SLUB_DEBUG/
#     KALLSYMS_ALL. A defconfig cut down to this board (no nouveau, no
#     Mellanox/Broadcom 10G, no Tegra/MediaTek/Qualcomm audio, no
#     DEBUG_INFO) should roughly halve both the image and the time U-Boot
#     spends reading it off eMMC, and shortens driver init. Worth doing,
#     but it is a kernel fragment + a boot test per revision, not a
#     one-liner.
#
#  c) KERNEL_IMAGETYPES = "Image fitImage" (k3.inc) installs both
#     /boot/Image (48 MB) and /boot/fitImage (16 MB). This board's bootcmd
#     never takes the boot_fit path, IMAGE_BOOT_FILES doesn't include the
#     fitImage, and flash-tools only touches emmc.wic + the loader blobs -
#     so /boot/fitImage is 16 MB of rootfs nothing reads. Dropping
#     "fitImage" is safe as far as this repo goes; it is listed here rather
#     than applied because KERNEL_IMAGETYPES is machine-wide and rebuilds
#     the kernel.
#
#  d) 'quiet loglevel=3' on the kernel command line (optargs in uEnv.txt).
#     stdout-path is main_uart0 at 115200 baud, and the kernel blocks on
#     that UART for every printk it emits during boot. Silencing it is
#     usually worth a second or more, at the cost of the boot log a
#     developer wants on a bench unit - so this belongs in a production-vs-
#     development split of uEnv.txt, not switched on globally.
# ---------------------------------------------------------------------------
