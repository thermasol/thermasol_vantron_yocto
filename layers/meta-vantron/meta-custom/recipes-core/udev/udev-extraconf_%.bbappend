FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://99-drm.rules"

do_install:append() {
    install -d ${D}${sysconfdir}/udev/rules.d
    install -m 0644 ${WORKDIR}/99-drm.rules ${D}${sysconfdir}/udev/rules.d/
}
