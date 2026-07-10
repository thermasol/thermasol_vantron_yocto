# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "apt configuration"
DESCRIPTION = "apt configuration"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RDEPENDS:${PN} += " gnupg"

SRC_URI = "file://vtlinux.list \
           file://vtlinux-archive-2021.gpg \
           "

S = "${WORKDIR}"

do_compile[noexec] = "1"

do_install () {
    install -d ${D}/${sysconfdir}/apt/sources.list.d
    install -d ${D}/${sysconfdir}/apt/trusted.gpg.d
    install ${S}/vtlinux.list ${D}/${sysconfdir}/apt/sources.list.d
    install ${S}/vtlinux-archive-2021.gpg ${D}/${sysconfdir}/apt/trusted.gpg.d
}
