# Copyright (C) 2021,Vantron - All Rights Reserved

DESCRIPTION = "Basic networkd configuration"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
DEPENDS = "systemd"

SRC_URI = " \
    file://98-net-alias.rules \
    file://net-alias-udev.sh \
    "

do_install() {
    # install link creation
    install -d ${D}${sysconfdir}/udev/rules.d/
    install -m 0644 ${WORKDIR}/98-net-alias.rules ${D}${sysconfdir}/udev/rules.d/
    install -d ${D}${sbindir}/
    install -m 0755 ${WORKDIR}/net-alias-udev.sh ${D}${sbindir}/

}

FILES:${PN} += "${systemd_unitdir}/network"
