SUMMARY = "Display rotation configuration for SignaSteam"
DESCRIPTION = "Provides EGLFS KMS configuration management for Qt5 display rotation based on application settings"
LICENSE = "CLOSED"

SRC_URI = "file://set-display-rotation.sh \
           file://eglfs_kms.json.default \
           file://rotation.env.default \
           file://backlight-boot.sh \
           file://backlight-boot.service \
          "

# The kernel brings the panel up dark (DT default-brightness-level 0): the
# backlight boost's cold inrush must not land on the panel's display-on
# surge on a 5 V long-cable feed (see recipes-kernel/linux). This unit
# raises it to the product level, through the driver's ramped enable, once
# psplash has drawn and the boot's first burst is over.
inherit systemd
SYSTEMD_SERVICE:${PN} = "backlight-boot.service"
SYSTEMD_AUTO_ENABLE = "enable"

S = "${WORKDIR}"

do_install() {
    # Install rotation script
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/set-display-rotation.sh ${D}${bindir}/
    install -m 0755 ${WORKDIR}/backlight-boot.sh ${D}${bindir}/
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/backlight-boot.service ${D}${systemd_system_unitdir}/

    # Install default EGLFS config and rotation environment
    install -d ${D}${sysconfdir}/qt5
    install -m 0644 ${WORKDIR}/eglfs_kms.json.default ${D}${sysconfdir}/qt5/eglfs_kms.json
    install -m 0644 ${WORKDIR}/rotation.env.default ${D}${sysconfdir}/qt5/rotation.env
}

FILES:${PN} += "${bindir}/set-display-rotation.sh ${bindir}/backlight-boot.sh ${systemd_system_unitdir}/backlight-boot.service"
FILES:${PN} += "${sysconfdir}/qt5/eglfs_kms.json"
FILES:${PN} += "${sysconfdir}/qt5/rotation.env"

RDEPENDS:${PN} += "bash"
