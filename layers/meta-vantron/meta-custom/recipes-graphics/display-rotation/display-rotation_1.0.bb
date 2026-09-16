SUMMARY = "Display rotation configuration for SignaSteam"
DESCRIPTION = "Provides EGLFS KMS configuration management for Qt5 display rotation based on application settings"
LICENSE = "CLOSED"

SRC_URI = "file://set-display-rotation.sh \
           file://eglfs_kms.json.default \
           file://rotation.env.default \
          "

S = "${WORKDIR}"

do_install() {
    # Install rotation script
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/set-display-rotation.sh ${D}${bindir}/

    # Install default EGLFS config and rotation environment
    install -d ${D}${sysconfdir}/qt5
    install -m 0644 ${WORKDIR}/eglfs_kms.json.default ${D}${sysconfdir}/qt5/eglfs_kms.json
    install -m 0644 ${WORKDIR}/rotation.env.default ${D}${sysconfdir}/qt5/rotation.env
}

FILES:${PN} += "${bindir}/set-display-rotation.sh"
FILES:${PN} += "${sysconfdir}/qt5/eglfs_kms.json"
FILES:${PN} += "${sysconfdir}/qt5/rotation.env"

RDEPENDS:${PN} += "bash"
