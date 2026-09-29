FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://99-drm.rules"

do_install:append() {
    install -d ${D}${sysconfdir}/udev/rules.d
    install -m 0644 ${WORKDIR}/99-drm.rules ${D}${sysconfdir}/udev/rules.d/

    # Drop the automount and autonet rules and the scripts behind them.
    #
    # automount.rules runs mount.sh for every block device and
    # autonet.rules runs network.sh for every net interface, synchronously
    # inside a udev worker. On this board that is ten block devices
    # (mmcblk0, its two boot areas, three partitions, four loop devices)
    # and can0, and mount.sh sat in systemd-mount for 4.5 s creating
    # /run/media/boot-mmcblk0p1 and /run/media/userfs-mmcblk0p3 - mounts
    # of partitions that are already mounted at /boot and /usr/local.
    # Measured with udevadm's USEC_INITIALIZED: udev processed 48 devices
    # at 7.2 s, then nothing until 11.2 s, then the remaining 67 - every
    # worker was blocked. The display chain, the ADC and everything else
    # udev had not reached yet waited those four seconds.
    #
    # Nothing in this image uses either: systemd-mount-partitions handles
    # the fixed partitions and systemd-networkd handles can0. There is no
    # removable media. localextra.rules (the /dev/fb symlink and the input
    # group) and our 99-drm.rules stay.
    rm -f ${D}${sysconfdir}/udev/rules.d/automount.rules \
          ${D}${sysconfdir}/udev/rules.d/autonet.rules \
          ${D}${sysconfdir}/udev/scripts/mount.sh \
          ${D}${sysconfdir}/udev/scripts/network.sh \
          ${D}${sysconfdir}/udev/mount.ignorelist
    rmdir --ignore-fail-on-non-empty ${D}${sysconfdir}/udev/scripts \
          ${D}${sysconfdir}/udev/mount.ignorelist.d 2>/dev/null || true
}
