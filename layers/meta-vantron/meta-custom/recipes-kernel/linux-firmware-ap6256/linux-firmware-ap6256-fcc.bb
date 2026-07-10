# Copyright (C) 2021,Vantron - All Rights Reserved
SUMMARY = "this is ap6256 firmware package"
HOMEPAGE = "https://www.vantrontech.com.cn/"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

SRC_URI:append = " \
	file://BCM4345C5.hcd \
    file://fw_bcm43456c5_ag.bin \
    file://nvram_ap6256.txt \
    file://ap6256bt.service \
    file://ap6256bt \
    file://wl \
    "

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = "ap6256bt.service"
SYSTEMD_AUTO_ENABLE = "enable"
inherit systemd

do_install:append () {
    install -d ${D}/${base_libdir}/firmware/bcmdhd

    install -m 644  ${WORKDIR}/BCM4345C5.hcd ${D}/${base_libdir}/firmware/bcmdhd
    install -m 644  ${WORKDIR}/fw_bcm43456c5_ag.bin ${D}/${base_libdir}/firmware/bcmdhd
    install -m 644  ${WORKDIR}/nvram_ap6256.txt ${D}/${base_libdir}/firmware/bcmdhd

	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/ap6256bt.service ${D}${systemd_unitdir}/system

	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/ap6256bt ${D}${bindir}/ap6256bt
    install -m 0755  ${WORKDIR}/wl ${D}/${bindir}/wl
}

FILES:${PN} += " ${systemd_unitdir}/system/vtvdm.service \ 
    /usr/lib/firmware/bcmdhd/* \
    ${bindir}/ap6275bt"

INSANE_SKIP:${PN} = "already-stripped arch"