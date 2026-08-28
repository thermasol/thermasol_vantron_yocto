SYSTEMD_ARAGO = ""
SYSTEMD_ARAGO:arago = "systemd-arago.inc"

require ${SYSTEMD_ARAGO}

FILESEXTRAPATHS:prepend := "${THISDIR}/systemd:"

SRC_URI:append = " file://25-can0.network"

do_install:append() {
    install -d ${D}${sysconfdir}/systemd/network/
    install -m 0644 ${WORKDIR}/25-can0.network ${D}${sysconfdir}/systemd/network/
}
