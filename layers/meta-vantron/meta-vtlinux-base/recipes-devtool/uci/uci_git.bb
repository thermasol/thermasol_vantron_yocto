DESCRIPTION = "OpenWrt Unified Configuration Interface"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://libuci.c;endline=13;md5=0ee862ed12171ee619c8c2eb7eff77f2"

DEPENDS = "libubox"
RDEPENDS:${PN} += "libubox"

SRC_URI = " \
    git://git.openwrt.org/project/uci.git;protocol=https;branch=master \
    file://0001-version-libraries.patch \
    "

S = "${WORKDIR}/git"

SRCREV = "f84f49f00fb70364f58b4cce72f1796a7190d370"
PV = "1.0.1+git${SRCPV}"

inherit cmake

EXTRA_OECMAKE = "-DBUILD_LUA=off -DABIVERSION=1.0.1"
