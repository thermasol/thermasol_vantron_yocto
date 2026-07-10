# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "Framework core components (LSB)"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

PROVIDES = "${PACKAGES}"
PACKAGES = " \
    packagegroup-framework-base          \
    packagegroup-framework-lsb-core      \
    packagegroup-framework-update        \
    packagegroup-framework-extra         \
    packagegroup-framework-network       \
    packagegroup-framework-database      \
    packagegroup-framework-audio         \
    packagegroup-framework-misc          \
    "

RDEPENDS:packagegroup-framework-base = " \
    packagegroup-framework-lsb-core      \
    packagegroup-framework-update        \
    packagegroup-framework-extra         \
    packagegroup-framework-network       \
    packagegroup-framework-database      \
    packagegroup-framework-audio         \
    packagegroup-framework-misc          \
    "

# Copy/Paste from packagegroup-core-lsb.bb
SUMMARY:packagegroup-framework-lsb-core = "LSB Core"
RDEPENDS:packagegroup-framework-lsb-core = " \
    at                \
    dash              \
    bash              \
    coreutils         \
    cpio              \
    cronie            \
    glibc-utils       \
    file              \
    findutils         \
    gawk              \
    grep              \
    gzip              \
    localedef         \
    lsb-release       \
    procps            \
    psmisc            \
    sed               \
    tar               \
    time              \
    util-linux        \
    \
    glibc             \
    libgcc            \
    zlib              \
    nspr              \
    nss               \
    apt-conf          \
    firmwared         \
    "

SUMMARY:packagegroup-framework-lsb-extra = "LSB extra"
RDEPENDS:packagegroup-framework-lsb-extra = " \
    bc                \
    ed                \
    cups              \
    fontconfig-utils  \
    man               \
    man-pages         \
    ghostscript       \
    diffutils         \
    elfutils          \
    msmtp             \
    patch             \
    libxml2           \
    ncurses           \
    can-utils         \
    "

SUMMARY:packagegroup-framework-update = "About SWupdate"
RDEPENDS:packagegroup-framework-update = " \
    u-boot-default-env \
    "

SUMMARY:packagegroup-framework-extra = "tools extra"
RDEPENDS:packagegroup-framework-extra = " \
    util-linux-lscpu  \
    util-linux-blkid  \
    memtester         \
    minicom           \
    i2c-tools         \
    mmc-utils         \
    usbutils          \
    libgpiod          \
    libgpiod-tools    \
    libiio-iiod       \
    libiio-tests      \
    parted            \
    gptfdisk          \
    e2fsprogs         \
    e2fsprogs-resize2fs \
    ${@bb.utils.contains('COMBINED_FEATURES', 'alsa', 'alsa-state', '', d)}           \
    ${@bb.utils.contains('COMBINED_FEATURES', 'alsa', 'alsa-utils-amixer', '', d)}    \
    ${@bb.utils.contains('COMBINED_FEATURES', 'alsa', 'alsa-utils-aplay', '', d)}     \
    "

SUMMARY:packagegroup-framework-network = "tools network tools"
RDEPENDS:packagegroup-framework-network = " \
    tcpdump          \
    openssh-sftp     \
    openssh-sftp-server \
    tzdata           \
    iperf3           \
    ${@bb.utils.contains('DISTRO_FEATURES', 'nfs', 'nfs-utils-mount ', '', d)}        \
    ${@bb.utils.contains('COMBINED_FEATURES', 'wifi', 'hostapd', '', d)}              \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'dhcpcd', '', d)}                       \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'systemd-networkd-configuration', '', d)}    \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'systemd-mount-partitions', '', d)}          \
    "

SUMMARY:packagegroup-framework-database = "about database"
RDEPENDS:packagegroup-framework-database = " \
    db               \
    sqlite3          \
    "

SUMMARY:packagegroup-framework-audio = "About audio"
RDEPENDS:packagegroup-framework-audio = "\
    ${@bb.utils.contains('DISTRO_FEATURES', 'pulseaudio', 'pulseaudio', '', d)}                     \
    ${@bb.utils.contains('DISTRO_FEATURES', 'pulseaudio', 'pulseaudio-server', '', d)}              \
    ${@bb.utils.contains('DISTRO_FEATURES', 'pulseaudio', 'pulseaudio-misc', '', d)}                \
    ${@bb.utils.contains('DISTRO_FEATURES', 'pulseaudio', 'pulseaudio-module-combine-sink', '', d)} \
    "

SUMMARY:packagegroup-framework-misc = "about misc"
RDEPENDS:packagegroup-framework-misc = " \
    resize-helper    \
    "
