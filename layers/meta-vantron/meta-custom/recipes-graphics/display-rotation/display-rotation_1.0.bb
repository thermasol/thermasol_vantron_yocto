SUMMARY = "Display rotation configuration for SignaSteam"
DESCRIPTION = "Provides EGLFS KMS configuration management for Qt5 display rotation based on application settings"
LICENSE = "CLOSED"

SRC_URI = "file://set-display-rotation.sh \
           file://eglfs_kms.json.default \
          "

S = "${WORKDIR}"

do_install() {
    # Install rotation script
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/set-display-rotation.sh ${D}${bindir}/

    # Install default EGLFS config
    install -d ${D}${sysconfdir}/qt5
    install -m 0644 ${WORKDIR}/eglfs_kms.json.default ${D}${sysconfdir}/qt5/eglfs_kms.json
}

FILES:${PN} += "${bindir}/set-display-rotation.sh"
FILES:${PN} += "${sysconfdir}/qt5/eglfs_kms.json"

RDEPENDS:${PN} += "bash"
