
IMAGE_FEATURES += "package-management splash"

# 4KB per 1 inode should be enough
EXTRA_IMAGECMD:ext2.gz += "-i 4096"

IMAGE_INSTALL = " \
	packagegroup-core-boot \
	${CORE_IMAGE_EXTRA_INSTALL} \
"

IMAGE_LINGUAS = ""

TOOLCHAIN_HOST_TASK:append = " nativesdk-buildtools-perl-dummy"
SDK_PACKAGE_ARCHS += "buildtools-dummy-${SDKPKGSUFFIX}"

BAD_RECOMMENDATIONS += " \
    libulm-dev \
    gdbserver-c6x-dev \
    coreutils \
    coreutils-dev \
"
BAD_RECOMMENDATIONS += "${@oe.utils.conditional("INIT_MANAGER", "sysvinit", "", "busybox-syslog", d)}"

# we're assuming some display manager is being installed with opengl
SYSTEMD_DEFAULT_TARGET = "${@bb.utils.contains('DISTRO_FEATURES','opengl','graphical.target','multi-user.target',d)}"

# Disable ubi/ubifs as the filesystem requires more space than is
# available on the HW.
IMAGE_FSTYPES:remove:omapl138 = "ubifs ubi"

DEVTOOLS = " \
    linux-libc-headers-dev \
    build-essential \
    packagegroup-core-tools-debug \
    git \
    dtc \
"

# Copyright (C) 2021,Vantron - All Rights Reserved
CORE_IMAGE_EXTRA_INSTALL += " \
    packagegroup-arago-base \
    packagegroup-arago-console \
    ${@bb.utils.contains('DISTRO_FEATURES','opengl','packagegroup-arago-tisdk-graphics','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','opengl','packagegroup-arago-tisdk-gtk','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','opencl','packagegroup-arago-opencl','',d)} \
    ${@bb.utils.contains('DISTRO_FEATURES','vulkan','packagegroup-arago-vulkan','',d)} \
    packagegroup-arago-tisdk-connectivity \
    packagegroup-arago-tisdk-crypto \
    packagegroup-arago-tisdk-multimedia \
    packagegroup-arago-tisdk-addons \
    packagegroup-arago-tisdk-addons-extra \
    ${DEVTOOLS} \
    ${@bb.utils.contains('TUNE_FEATURES', 'armv7a', 'valgrind', '', d)} \
    packagegroup-core-full-cmdline \
    packagegroup-tools-bluetooth \
    gstreamer1.0-rtsp-server \
    gstreamer1.0-libav \
    firmwared \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 wayland', 'weston-xwayland xterm', '', d)} \
    \
    mpv \
    libvtcom \
    libvtcom-dev \
    eeprom-link \
    packagegroup-tools-bluetooth \
    hardwatchdog \
    hardwatchdog-script \
    wireless-regdb-static \
    udev-extraconf \
    openssh \
    openssh-sshd \
    brcm-patchram-plus \
    linux-firmware-ap6256 \
    triggerhappy \
    tslib \
    android-tools-adbd \
    android-tools-conf-ti \
    evtest \
    glibc-gconv-utf-16 \
    bluez-tools \
    bluealsa \
    "

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

