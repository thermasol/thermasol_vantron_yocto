SUMMARY = "Minimal VTLinux Qt5 EGLFS image for SignaTouch IR"
DESCRIPTION = "Minimal embedded Linux image with Qt5 EGLFS backend and SignaTouch IR application"
LICENSE = "MIT"

inherit core-image

# Single-app kiosk slimming policy (package trimming, kernel-module
# allowlist, service masking, filesystem sizing). Must come after
# "inherit core-image" so its IMAGE_INSTALL:remove lines apply to the
# IMAGE_INSTALL the class has already seeded. See the file for what each
# knob costs and for the phase-2 items that are deliberately left off.
require vtlinux-image-kiosk.inc

# No SSH server: the board has no network interface in this product (no
# Ethernet, Wi-Fi driver not shipped), so sshd was a listener with nothing
# to listen on and one more root login path to account for.
#
# ADB stays up at boot on this product (KIOSK_ADB_AT_BOOT default "1"):
# the IR app has no hidden enable gesture like SignaSteam's Diagnostics
# screen, and adbd is the field-service path. Root still has the password
# from vtlinux-image-kiosk.inc on the serial console.

# Essential Qt5 packages for Widgets application
QT5_PACKAGES = " \
    qtbase \
    qtbase-plugins \
    qtsvg \
"

# Graphics/OpenGL packages for EGLFS.
#
# mesa-megadriver is the llvmpipe software rasteriser and drags in
# clang-libllvm (99 MB) - by far the largest thing in the image. It stays
# only because qtbase is currently built with eglfs/gles2 and so RDEPENDS
# on libegl-mesa/libgles2-mesa. See phase-2 item (a) in
# vtlinux-image-kiosk.inc for the linuxfb swap that removes all of it;
# SignaTouch_IR is pure QWidget and needs no GL.
#
# libdrm-tests and kmscube were bring-up tools, not runtime dependencies -
# they now live behind KIOSK_DEBUG_TOOLS.
GRAPHICS_PACKAGES = " \
    mesa \
    mesa-megadriver \
    libegl-mesa \
    libgbm \
    libdrm \
"

# Fonts for Qt. The app draws with the default sans face only; the mono and
# serif faces were 3 MB of unreferenced glyphs. fontconfig-utils (fc-list
# and friends) is a diagnostic tool, not a runtime dependency - it moved
# behind KIOSK_DEBUG_TOOLS.
FONT_PACKAGES = " \
    fontconfig \
    ttf-dejavu-sans \
"

# Core system packages.
#
# packagegroup-framework-base is dropped in favour of an explicit list.
# That packagegroup is a general-purpose LSB/server userland - it pulls
# nss+nspr (15 MB), file/libmagic (8.5 MB), coreutils (9.6 MB), cronie,
# at, localedef, lsb-release, glibc-utils, gawk, cups/ghostscript/man via
# its -extra group, apt-conf -> gnupg (5 MB), firmwared, tcpdump, iperf3,
# hostapd, avahi, sqlite3+db, memtester/minicom/usbutils/parted/gptfdisk
# and libiio - none of which SignaTouch_IR links against or execs. The
# pieces that are genuinely load-bearing on this board are named below;
# busybox (from packagegroup-core-boot) provides the shell utilities the
# startup scripts use.
#
# kernel-modules is dropped in favour of the three-module allowlist in
# vtlinux-image-kiosk.inc.
CORE_PACKAGES = " \
    can-utils \
    tzdata \
    dhcpcd \
    systemd-networkd-configuration \
    systemd-mount-partitions \
    resize-helper \
    u-boot-default-env \
    util-linux-blkid \
    libgpiod \
"

# Hardware support.
#
# brcm-patchram-plus, linux-firmware-ap6256, packagegroup-tools-bluetooth,
# bluez-tools, wireless-regdb-static and the Wi-Fi/Bluetooth kernel modules
# now sit behind KIOSK_WIFI / KIOSK_BLUETOOTH in vtlinux-image-kiosk.inc,
# defaulted off. Flip those to "1" if a unit needs wireless.
#
# triggerhappy is a hotkey daemon for a device with no keyboard; tslib is
# the resistive-touch calibration stack, unused here (the chsc5xxx
# controller reports absolute multitouch straight to evdev and Qt's
# eglfs/linuxfb input plugin reads evdev directly).
#
# rail-5v: the shared kernel DT leaves the switched 5 V rail (CAN
# transceiver supply) off through the boot, because closing that switch is
# what browned the board out on a long 5 V cable (recipes-kernel/linux).
# rail-5v.service turns it on once the system is up; without this package
# can0 never gets a transceiver and the IR app sees no bus.
HARDWARE_PACKAGES = " \
    hardwatchdog \
    hardwatchdog-script \
    udev-extraconf \
    rail-5v \
"

# Vendor libraries. libvtcom-dev is a -dev package (headers and the
# unversioned .so); it has no business in a target rootfs.
VENDOR_PACKAGES = " \
    libvtcom \
    vtvdm \
    eeprom-link \
"

# Your application
APP_PACKAGES = " \
    signatouch-ir \
    display-rotation \
"

CORE_IMAGE_EXTRA_INSTALL += " \
    ${QT5_PACKAGES} \
    ${GRAPHICS_PACKAGES} \
    ${FONT_PACKAGES} \
    ${CORE_PACKAGES} \
    ${HARDWARE_PACKAGES} \
    ${VENDOR_PACKAGES} \
    ${APP_PACKAGES} \
"

# Configure Qt5 for EGLFS (no Wayland/Weston needed)
PACKAGECONFIG:append:pn-qtbase = " eglfs gles2 kms gbm evdev"
PACKAGECONFIG:remove:pn-qtbase = "xcb wayland"

# WKS file for disk partitioning
WKS_FILE_DEPENDS = " \
    vtlinux-image-userfs \
"
WKS_FILE = "${MACHINE}.wks.in"

LICENSE_FLAGS_ACCEPTED += "commercial"

IMAGE_PREPROCESS_COMMAND:append = "rootfs_versions;"

rootfs_versions () {
    echo ${IMAGE_VERSION} > ${IMAGE_ROOTFS}/etc/versions
    bbnote "rootfs_reproducible: set /etc/versions to ${IMAGE_VERSION}"
}

# The local display is owned by the boot splash and then the app - a getty
# login prompt on tty1 (same physical framebuffer) paints its /etc/issue
# banner and "login:" prompt right over whatever psplash/the app drew
# there (confirmed on-device with SignaSteam). Serial debug access via
# ttyS0 is unaffected since it's a separate console.
ROOTFS_POSTPROCESS_COMMAND += "mask_display_getty ; "

mask_display_getty () {
    install -d ${IMAGE_ROOTFS}/etc/systemd/system
    ln -sf /dev/null ${IMAGE_ROOTFS}/etc/systemd/system/getty@tty1.service
}
