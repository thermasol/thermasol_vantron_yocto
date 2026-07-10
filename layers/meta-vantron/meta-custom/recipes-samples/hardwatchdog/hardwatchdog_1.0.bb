# Copyright (C) 2021,Vantron - All Rights Reserved
SUMMARY = "this is power check for VT-M08-A Software"
HOMEPAGE = "https://www.vantrontech.com.cn/"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

# FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI = " \
    file://hardwatchdog.service \
    file://hardwatchdog.sh \
    file://hardwatchdog-user.sh \
    "

SYSTEMD_PACKAGES = "${PN}-script"
SYSTEMD_SERVICE:${PN}-script = "hardwatchdog.service"
SYSTEMD_AUTO_ENABLE:${PN}-script = "enable"

do_install () {
	
    install -d ${D}${systemd_system_unitdir}
    install -m 644 ${WORKDIR}/hardwatchdog.service ${D}${systemd_system_unitdir}/hardwatchdog.service
   
    install -d ${D}${exec_prefix}/bin/
    install -m 755 ${WORKDIR}/hardwatchdog.sh ${D}${exec_prefix}/bin/
    install -m 755 ${WORKDIR}/hardwatchdog-user.sh ${D}${exec_prefix}/bin/
}
inherit systemd

PACKAGES =+ "${PN}-script"

FILES:${PN}-script += " ${systemd_system_unitdir}"
FILES:${PN} += "/usr/bin/* "

# No GNU_HASH in the elf binary
TARGET_CC_ARCH += "${LDFLAGS}"
INSANE_SKIP_${PN} = "ldflags"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"

INSANE_SKIP_${PN} += "file-rdeps libdir"
INSANE_SKIP_${PN}-dev += "dev-elf libdir"

RDEPENDS_${PN} += "bash"
