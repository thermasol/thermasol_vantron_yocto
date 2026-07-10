SUMMARY = "create eeprom link"
HOMEPAGE = "https://www.vantrontech.com.cn/"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

SRC_URI = "file://eeprom.service"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = "eeprom.service"
SYSTEMD_AUTO_ENABLE = "enable"
inherit systemd

RDEPENDS:${PN}=" vtvdm "

do_install() {
	install -d ${D}${systemd_system_unitdir}
	install -m 0644 ${WORKDIR}/eeprom.service ${D}${systemd_system_unitdir}
}

FILES:${PN} += "${systemd_system_unitdir}/eeprom.service"
