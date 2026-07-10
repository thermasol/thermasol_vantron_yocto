# Copyright (C) 2021,Vantron - All Rights Reserved

DESCRIPTION = "Gstreamer 1.0 components"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

PROVIDES = "${PACKAGES}"
PACKAGES = " \
    packagegroup-framework-gstreamer      \
    packagegroup-framework-gstreamer-misc \
    "

RDEPENDS:packagegroup-framework-gstreamer = " \
    packagegroup-framework-gstreamer-misc \
    "

SUMMARY:packagegroup-framework-gstreamer-misc = "gstreamer misc"
RDEPENDS:packagegroup-framework-gstreamer-misc = " \
    gstreamer1.0                         \
    gstreamer1.0-libav                   \
    gstreamer1.0-meta-base               \
    gstreamer1.0-omx                     \
    gstreamer1.0-plugins-bad             \
    gstreamer1.0-plugins-base            \
    gstreamer1.0-plugins-good            \
    gstreamer1.0-plugins-ugly            \
    gstreamer1.0-rtsp-server             \
    "
