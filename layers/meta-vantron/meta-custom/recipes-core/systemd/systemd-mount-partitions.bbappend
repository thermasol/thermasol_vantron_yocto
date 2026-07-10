# Copyright (C) 2021,Vantron - All Rights Reserved
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SUMMARY = "Mount partitions"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RDEPENDS:${PN} += " util-linux "

SRC_URI += " \
    file://mount-partitions.sh    \
    "

inherit systemd

SYSTEMD_PACKAGES = "${@bb.utils.contains('DISTRO_FEATURES','systemd','${PN}','',d)}"
SYSTEMD_SERVICE:${PN} = "mount-partitions.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install:append () {
    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${systemd_unitdir}/system ${D}/${base_sbindir}
        install -m 755 ${WORKDIR}/mount-partitions.sh ${D}/${base_sbindir}/
    fi
}

FILES:${PN} += " ${systemd_unitdir} ${base_sbindir} ${INIT_D_DIR}"
