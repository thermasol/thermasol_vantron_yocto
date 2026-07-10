# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "Framework qt components over QTwayland"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit packagegroup

RDEPENDS:${PN} = " \
    qtwayland                    \
    qtwayland-plugins            \
    vtlinux-qt-wayland           \
    "
