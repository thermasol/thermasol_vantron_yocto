SUMMARY = "Command-line switch for the board's GPIO-switched 5 V rail"
DESCRIPTION = "The kernel leaves the switched 5 V rail (gpio_5v_en) off through \
the boot because closing that switch is the board's biggest load step. \
rail-5v.service switches it on once the panel is up, before the backlight \
and before systemd-networkd configures can0 (the CAN transceiver is on this \
rail); `rail-5v on|off|status` drives it by hand."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://rail-5v \
           file://rail-5v.service \
          "

inherit systemd

SYSTEMD_SERVICE:${PN} = "rail-5v.service"
SYSTEMD_AUTO_ENABLE = "enable"
S = "${WORKDIR}"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/rail-5v ${D}${bindir}/rail-5v
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/rail-5v.service ${D}${systemd_system_unitdir}/
}

FILES:${PN} += "${bindir}/rail-5v ${systemd_system_unitdir}/rail-5v.service"
