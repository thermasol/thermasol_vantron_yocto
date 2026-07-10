# Copyright (C) 2021,Vantron - All Rights Reserved

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

PACKAGECONFIG:append = " \
    firstboot \
    coredump \
    iptc \
    "

do_install:append() {
    #Remove this service useless for our needs
    rm -f ${D}/${rootlibexecdir}/systemd/system-generators/systemd-gpt-auto-generator

    # Disable the assignment of the fixed network interface name
    # install -d ${D}${sysconfdir}/systemd/network
    # ln -s /dev/null ${D}${sysconfdir}/systemd/network/99-default.link
}

