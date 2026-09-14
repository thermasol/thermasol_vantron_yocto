SUMMARY = "Minimal VTLinux Qt5 EGLFS image for SignaTouch"
DESCRIPTION = "Minimal embedded Linux image with Qt5 EGLFS backend and SignaTouch application"
LICENSE = "MIT"

inherit core-image

# Single-app kiosk slimming policy (package trimming, kernel-module
# allowlist, service masking, filesystem sizing). Must come after
# "inherit core-image" so its IMAGE_INSTALL:remove lines apply to the
# IMAGE_INSTALL the class has already seeded. See the file for what each
# knob costs and for the phase-2 items that are deliberately left off.
require vtlinux-image-kiosk.inc

IMAGE_FEATURES += " \
    ssh-server-openssh \
"

# Essential Qt5 packages for Widgets application. No qtsvg: SignaSteam's
# resources.qrc is PNG and TTF only (the IR image needs it for
# arcsliderplugin, this one doesn't).
QT5_PACKAGES = " \
    qtbase \
    qtbase-plugins \
"

# Graphics/OpenGL packages for EGLFS.
#
# mesa-megadriver is the llvmpipe software rasteriser and drags in
# clang-libllvm (99 MB) - by far the largest thing in the image. It stays
# only because qtbase is currently built with eglfs/gles2 and so RDEPENDS
# on libegl-mesa/libgles2-mesa. See phase-2 item (a) in
# vtlinux-image-kiosk.inc for the linuxfb swap that removes all of it;
# SignaSteam is pure QWidget and needs no GL.
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
# and libiio - none of which SignaSteam links against or execs. The
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
HARDWARE_PACKAGES = " \
    hardwatchdog \
    hardwatchdog-script \
    udev-extraconf \
"

# Vendor libraries. libvtcom-dev is a -dev package (headers and the
# unversioned .so); it has no business in a target rootfs.
VENDOR_PACKAGES = " \
    libvtcom \
    vtvdm \
    eeprom-link \
"

# Your application.
#
# display-rotation is load-bearing here, not optional as it is on the IR
# unit: SignaSteam has a user-selectable orientation (the first-boot wizard
# and Settings > Device > Orientation), it stores the choice as
# orientationSelected in /opt/ThermaSol/SignaSteam.conf, and
# set-display-rotation.sh is what turns that into the EGLFS rotation at the
# next boot. It also forces the panel backlight to full brightness.
APP_PACKAGES = " \
    signatouch \
    display-rotation \
"

# Room-temperature sensor.
#
# The app reads the thermistor divider on ADC channel 0 through IIO sysfs
# (/sys/bus/iio/devices/iio:device0/in_voltage0_raw, see
# EquipmentCanInterface::onTempSensorTimerTimeOut) and converts it with a
# Steinhart/B-factor curve. What has to be true for that node to exist:
#
#   DT      tscadc0 is status = "okay" with ti,adc-channels = <0 1> and
#           adc_pins_default, in vt-sbc-am62l.dtsi - already the case.
#   Kernel  CONFIG_IIO=y, CONFIG_IIO_BUFFER=y, and
#           CONFIG_MFD_TI_AM335X_TSCADC=m / CONFIG_TI_AM335X_ADC=m /
#           CONFIG_IIO_KFIFO_BUF=m in vt_sbc_am62l_defconfig.
#   Rootfs  the three module packages below. kernel-module-ti-am335x-adc
#           is already in KIOSK_MODULES_SENSORS in vtlinux-image-kiosk.inc
#           and pulls the other two via kernel-module-split's modinfo
#           RDEPENDS; they are named again here so that a future edit to
#           the shared kiosk allowlist can't silently take the temperature
#           reading out of this product.
#   Boot    /etc/modules-load.d/ti-adc.conf, shipped by the signatouch
#           recipe, so the node is there before the app's first read
#           instead of depending on udev coldplug timing.
SENSOR_PACKAGES = " \
    kernel-module-ti-am335x-adc \
    kernel-module-ti-am335x-tscadc \
    kernel-module-kfifo-buf \
"

CORE_IMAGE_EXTRA_INSTALL += " \
    ${QT5_PACKAGES} \
    ${GRAPHICS_PACKAGES} \
    ${FONT_PACKAGES} \
    ${CORE_PACKAGES} \
    ${HARDWARE_PACKAGES} \
    ${VENDOR_PACKAGES} \
    ${SENSOR_PACKAGES} \
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
# there (confirmed on-device). Serial debug access via
# ttyS0 is unaffected since it's a separate console.
ROOTFS_POSTPROCESS_COMMAND += "mask_display_getty; "

mask_display_getty () {
    install -d ${IMAGE_ROOTFS}/etc/systemd/system
    ln -sf /dev/null ${IMAGE_ROOTFS}/etc/systemd/system/getty@tty1.service
}
