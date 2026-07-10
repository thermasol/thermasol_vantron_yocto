# Copyright 2024 Vantrontech
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://reset.conf \
    file://check_resotre.sh \
    file://run_factory \
    "

do_install:prepend () {
    sed -i 's/ --user nobody//' ${S}/systemd/triggerhappy.service
}

do_install:append () {
    install -m 755 ${WORKDIR}/reset.conf ${D}${sysconfdir}/triggerhappy/triggers.d/reset.conf
    install -m 755 ${WORKDIR}/check_resotre.sh ${D}/usr/sbin/check_resotre.sh
    install -d ${D}/usr/bin/
    install -m 755 ${WORKDIR}/run_factory ${D}/usr/bin/run_factory
}
FILES:${PN} += "/usr/sbin/check_resotre.sh"
FILES:${PN} += "/usr/bin/run_factory"