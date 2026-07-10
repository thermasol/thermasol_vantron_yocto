# Copyright (C) 2021,Vantron - All Rights Reserved

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI +=  "file://libc.conf"

do_install:append() {
    install -d 644 ${D}/${sysconfdir}/ld.so.conf.d
    install -m 644 ${WORKDIR}/libc.conf ${D}/${sysconfdir}/ld.so.conf.d/libc.conf
}

FILES_ldconfig += "${sysconfdir}/ld.so.conf.d/libc.conf"

FILES:${PN} += " ${sysconfdir}/ld.so.conf.d/*"
