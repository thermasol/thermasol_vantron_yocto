# Copyright (C) 2016, STMicroelectronics - All Rights Reserved
# Copyright (C) 2021, Vantron
# Released under the MIT license (see COPYING.MIT for the terms)

# Tools extracted from 96boards-tools https://github.com/96boards/96boards-tools
# e2fsprogs for resize2fs
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

RDEPENDS:${PN} += "expect"
SRC_URI += "file://resize-helper \
    file://real-resize-helper \
    "

do_install:append () {
    install -m 0755 ${WORKDIR}/resize-helper ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/real-resize-helper ${D}${base_sbindir}
}

