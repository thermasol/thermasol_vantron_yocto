# Copyright (C) 2021,Vantron - All Rights Reserved

DESCRIPTION = "add script and material to help with eglfs qt configuration"
HOMEPAGE = "https://www.vantrontech.com.cn/"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

SRC_URI = " \
    file://qt-wayland.sh \
    "

S = "${WORKDIR}/sources"
UNPACKDIR = "${S}"

inherit allarch

do_install() {
    install -d ${D}/${sysconfdir}/profile.d

    install -m 0755 ${WORKDIR}/qt-wayland.sh ${D}/${sysconfdir}/profile.d/
}
RDEPENDS:${PN} = "qtwayland"
FILES:${PN} += "${datadir}/qt6"
