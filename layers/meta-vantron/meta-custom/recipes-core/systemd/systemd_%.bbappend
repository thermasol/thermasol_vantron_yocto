SYSTEMD_ARAGO = ""
SYSTEMD_ARAGO:arago = "systemd-arago.inc"

require ${SYSTEMD_ARAGO}

FILESEXTRAPATHS:prepend := "${THISDIR}/systemd:"

SRC_URI:append = " file://25-can0.network"

# udev-hwdb is systemd's compiled hardware database - 8.2 MB of PCI/USB
# vendor IDs and keyboard scancode maps. On fixed hardware with a built-in
# chsc5xxx touch controller and no keyboard nothing ever queries it, and
# systemd-hwdb-update leaves the boot with it.
#
# BAD_RECOMMENDATIONS is the usual way to drop this from an image, but
# PACKAGE_CLASSES here is package_deb and rootfs_deb.bbclass only warns
# ("Debian package install does not support BAD_RECOMMENDATIONS") - apt
# installs the Recommends: anyway. Dropping it at the source of the
# recommendation works regardless of package manager.
RRECOMMENDS:${PN}:remove = "udev-hwdb"

do_install:append() {
    install -d ${D}${sysconfdir}/systemd/network/
    install -m 0644 ${WORKDIR}/25-can0.network ${D}${sysconfdir}/systemd/network/
}
