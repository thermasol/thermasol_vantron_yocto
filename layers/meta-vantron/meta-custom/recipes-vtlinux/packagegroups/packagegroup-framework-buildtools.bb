# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "Framework buildtools"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

PROVIDES = "${PACKAGES}"
PACKAGES = " \
    packagegroup-framework-buildtools        \
    packagegroup-framework-buildtools-base   \
    "

RDEPENDS:packagegroup-framework-buildtools = " \
    packagegroup-framework-buildtools-base   \
    "

SUMMARY:packagegroup-framework-buildtools = "buildtools base"
RDEPENDS:packagegroup-framework-buildtools-base = " \
    m4                \
    autoconf          \
    automake          \
    binutils          \
    binutils-symlinks \
    cpp               \
    cpp-symlinks      \
    gcc               \
    gcc-symlinks      \
    g++               \
    g++-symlinks      \
    gettext           \
    make              \
    libstdc++         \
    libstdc++-dev     \
    libtool           \
    pkgconfig         \
    "

