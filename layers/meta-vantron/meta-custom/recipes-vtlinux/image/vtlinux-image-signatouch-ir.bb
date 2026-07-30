SUMMARY = "Minimal VTLinux Qt5 EGLFS image for SignaTouch IR"
DESCRIPTION = "Minimal embedded Linux image with Qt5 EGLFS backend and SignaTouch IR application"
LICENSE = "MIT"

inherit core-image

IMAGE_FEATURES += " \
    package-management \
    ssh-server-openssh \
"

# Essential Qt5 packages for Widgets application
QT5_PACKAGES = " \
    qtbase \
    qtbase-plugins \
    qtbase-tools \
    qtsvg \
"

# Graphics/OpenGL packages for EGLFS
GRAPHICS_PACKAGES = " \
    mesa \
    mesa-megadriver \
    libegl-mesa \
    libgbm \
    libdrm \
    libdrm-tests \
    kmscube \
"

# Fonts for Qt
FONT_PACKAGES = " \
    fontconfig \
    fontconfig-utils \
    ttf-dejavu-sans \
    ttf-dejavu-sans-mono \
    ttf-dejavu-serif \
"

# Core system packages
CORE_PACKAGES = " \
    packagegroup-framework-base \
    kernel-modules \
    can-utils \
"

# Development and debug tools
DEBUG_TOOLS = " \
    android-tools-adbd \
    android-tools-conf-ti \
    packagegroup-core-tools-debug \
    evtest \
"

# Hardware support
HARDWARE_PACKAGES = " \
    hardwatchdog \
    hardwatchdog-script \
    brcm-patchram-plus \
    linux-firmware-ap6256 \
    packagegroup-tools-bluetooth \
    bluez-tools \
    wireless-regdb-static \
    udev-extraconf \
    triggerhappy \
    tslib \
"

# Vendor libraries
VENDOR_PACKAGES = " \
    libvtcom \
    libvtcom-dev \
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
    ${DEBUG_TOOLS} \
    ${HARDWARE_PACKAGES} \
    ${VENDOR_PACKAGES} \
    ${APP_PACKAGES} \
"

# Configure Qt5 for EGLFS (no Wayland/Weston needed)
PACKAGECONFIG:append:pn-qtbase = " eglfs gles2 kms gbm evdev"
PACKAGECONFIG:remove:pn-qtbase = "xcb wayland"

# Set graphical target since we have a GUI app
SYSTEMD_DEFAULT_TARGET = "graphical.target"

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
ROOTFS_POSTPROCESS_COMMAND += "mask_display_getty; "

mask_display_getty () {
    install -d ${IMAGE_ROOTFS}/etc/systemd/system
    ln -sf /dev/null ${IMAGE_ROOTFS}/etc/systemd/system/getty@tty1.service
}
