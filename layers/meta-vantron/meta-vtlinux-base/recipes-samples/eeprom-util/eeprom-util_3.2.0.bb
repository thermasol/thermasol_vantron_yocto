# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "This utility displays and updates EEPROM information"
HOMEPAGE = "https://github.com/compulab/eeprom-util"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://LICENSE;md5=2c1c00f9d3ed9e24fa69b932b7e7aff2"

SRC_URI = " \
    git://github.com/compulab/eeprom-util.git;protocol=https;branch=master \
    file://Allow-cross-compilation.patch \
    "
SRCREV = "257db099e8339404510eded3521da5370b321041"

S = "${WORKDIR}/git"
B = "${WORKDIR}/git"

do_configure[noexec] = "1"

do_compile () {
    oe_runmake auto_generated.h
    oe_runmake write
}

do_install () {
    install -d ${D}${base_bindir}
    install -m 755 ${B}/eeprom-util ${D}${base_bindir}
}
