# Mirrored to GitHub -- see the matching note in linux-ti-staging_%.bbappend.
SRC_URI = "git://git@github.com/harvia-dev/thermasol_uboot-ti-am62l.git;protocol=ssh;branch=${SRCBRANCH}"

SRCBRANCH = "vt-sbc-am62l"
SRCREV = "bba251a2332e3a96157ae247020f6a7ff264ce25"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Default optargs. bootcmd is "run args_mmc; run loadimage; run loadfdt;
# run run_kern" and args_mmc builds bootargs as "console=${console}
# ${optargs} root=...". optargs has no default, and this bootcmd never
# loads uEnv.txt, so the "optargs=vt.global_cursor_default=0" that
# tisdk-uenv ships has never reached the kernel (confirmed: it is absent
# from /proc/cmdline). Now that the display comes up inside the kernel and
# fbcon draws the ThermaSol boot logo on the panel, the VT cursor would
# blink over the logo and over psplash, so this hides it, and the fbcon=
# options centre the logo where psplash draws it and stop fbcon drawing
# one copy per CPU.
SRC_URI += "file://0001-am62lx-env-default-optargs-hide-the-fbcon-cursor.patch"

# Keep the AP6256 Wi-Fi/BT module unpowered from SPL onwards. The image has
# no driver for it, and its load switch was being turned on at every boot
# (regulator-boot-on + a pull-up on the enable pad) for nothing - current
# the 5 V feed cannot spare during the boot peak on a long cable. Mirrors
# the kernel-side patch in recipes-kernel/linux; drop both for a build
# with KIOSK_WIFI / KIOSK_BLUETOOTH = "1".
SRC_URI += "file://0002-vt-sbc-am62l-dts-leave-the-wifi-bt-module-unpowered.patch"

# Autoboot abort. The defconfig has CONFIG_BOOTDELAY=0, which still lets a
# key held on the serial console during boot drop into the U-Boot prompt
# (from where init=/bin/sh is one line away). -2 is U-Boot's "no delay, do
# not check for a key": the production console cannot interrupt the boot.
# Reflashing still works, the flash tools boot the stock loader over USB
# DFU and never touch this. Set "3" in site.conf for a bench build where
# you want the prompt back (the hand-boot bisect in the kernel notes).
UBOOT_BOOTDELAY ?= "-2"
do_configure:prepend() {
    sed -i 's/^CONFIG_BOOTDELAY=.*/CONFIG_BOOTDELAY=${UBOOT_BOOTDELAY}/' ${S}/configs/${UBOOT_MACHINE}
    grep -q '^CONFIG_BOOTDELAY=${UBOOT_BOOTDELAY}$' ${S}/configs/${UBOOT_MACHINE} || \
        bbfatal "CONFIG_BOOTDELAY not set in ${UBOOT_MACHINE}"
}

PACKAGECONFIG[ap-trusted-rom] = "BL1=${S}/prebuilt-images/am62lxx-evm/bl1.bin,,trusted-firmware-a"
PACKAGECONFIG[atf] = "BL31=${S}/prebuilt-images/am62lxx-evm/bl31.bin,,trusted-firmware-a"
PACKAGECONFIG[optee] = "TEE=${S}/prebuilt-images/am62lxx-evm/bl32.bin,,optee-os"
