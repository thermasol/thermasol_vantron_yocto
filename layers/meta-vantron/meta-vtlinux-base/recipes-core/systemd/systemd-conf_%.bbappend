# Copyright (C) 2021,Vantron - All Rights Reserved

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " \
    file://coredump-custom.conf \
    "

do_install:prepend() {
    install -d ${D}${sysconfdir}/systemd/coredump.conf.d/
    install -m 644 ${WORKDIR}/coredump-custom.conf ${D}${sysconfdir}/systemd/coredump.conf.d/

    # ignore poweroff key on logind
    install -d ${D}${systemd_unitdir}/logind.conf.d/
    echo "[Login]" > ${D}${systemd_unitdir}/logind.conf.d/01-vtlinux.conf
    echo "HandlePowerKey=ignore" >> ${D}${systemd_unitdir}/logind.conf.d/01-vtlinux.conf

    # Journal, do not store journald on filesystem (syslog make it already)
    install -d ${D}${systemd_unitdir}/journald.conf.d/
    echo "[Journal]" > ${D}${systemd_unitdir}/journald.conf.d/01-vtlinux.conf
    echo "Storage=volatile" >> ${D}${systemd_unitdir}/journald.conf.d/01-vtlinux.conf
}
FILES:${PN} += " ${sysconfdir}/systemd/coredump.conf.d/ "
