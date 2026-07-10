DESCRIPTION = "OpenWrt system message/RPC bus"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://ubusd.c;endline=12;md5=1b6a7aecd35bdd25de35da967668485d"

DEPENDS = "libubox json-c"
RDEPENDS:${PN} += "libubox"

SRC_URI = " \
    git://git.openwrt.org/project/ubus.git;protocol=https;branch=master \
    file://ubusd.service \
    "

S = "${WORKDIR}/git"

SRCREV = "a72457b61df045d3c499a6211362b751710590d7"
PV = "1.0.1+git${SRCPV}"

inherit cmake systemd

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = "ubusd.service"

do_install:append () {
    install -d ${D}${systemd_system_unitdir}
    install -m 644 ${WORKDIR}/ubusd.service ${D}${systemd_system_unitdir}
}

EXTRA_OECMAKE = "-DBUILD_LUA=off -DBUILD_EXAMPLES=off -DABIVERSION=1.0.1"
