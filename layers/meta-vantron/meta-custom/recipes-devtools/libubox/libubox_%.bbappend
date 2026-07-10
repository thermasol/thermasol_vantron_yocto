
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI = "\
    git://git.openwrt.org/project/libubox.git;branch=master \
    file://fix-libdir2.patch \
"

SRCREV = "3868f47c8f6c6570e62a3cdf8a7f26ffb1a67e6a"

FILES:${PN}-dev = " \
    /usr/include/ \
"

FILES:${PN} += " /usr/lib/* "