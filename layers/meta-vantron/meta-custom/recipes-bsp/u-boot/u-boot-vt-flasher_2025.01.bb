SUMMARY = "USB-booted flasher U-Boot for vt-sbc-am62l with a fastboot gadget"
DESCRIPTION = "Second build of the board's U-Boot source, from Vantron's \
vt_sbc_am62l_dfu_defconfig (the configuration behind the stock loader the \
Flash Station boots over USB DFU) plus a fastboot gadget. It never goes on \
a unit: the station loads it into RAM at step 3 and the eMMC gets the \
production u-boot-ti-staging build as before. See flash-tools/README.md."

# Everything the production recipe gets from meta-ti, minus the bbappend
# in this directory (that one matches u-boot-ti-staging only, which is the
# point: this build gets its own bootcmd from the config fragment).
require recipes-bsp/u-boot/u-boot-ti.inc

PR = "r0"
BRANCH = "vt-sbc-am62l"
# Keep in step with SRCREV in u-boot-ti-staging_%.bbappend.
SRC_URI = "git://git@github.com/harvia-dev/thermasol_uboot-ti-am62l.git;protocol=ssh;branch=${BRANCH}"
SRCREV = "bba251a2332e3a96157ae247020f6a7ff264ce25"
PV = "2025.01+git"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:${THISDIR}/u-boot-vt-flasher:"
# Same source patches as the production build so the two stay one tree.
SRC_URI += "file://0001-am62lx-env-default-optargs-hide-the-fbcon-cursor.patch"
SRC_URI += "file://0002-vt-sbc-am62l-dts-leave-the-wifi-bt-module-unpowered.patch"
SRC_URI += "file://vt-flasher-fastboot.config"

UBOOT_MACHINE = "vt_sbc_am62l_dfu_defconfig"
# Merged by u-boot-mergeconfig.inc (make <defconfig> <fragment>). U-Boot's
# scripts/kconfig/Makefile looks a *.config fragment up in kernel/configs,
# arch/arm/configs and anywhere under board/ - not in configs/, where the
# defconfigs and their #include'd fragments live - so it goes next to the
# board code.
UBOOT_CONFIG_FRAGMENTS = "vt-flasher-fastboot.config"
do_configure:prepend() {
    install -m 0644 ${WORKDIR}/vt-flasher-fastboot.config ${S}/board/ti/am62lx/
}

# Not a bootloader for the image. The production u-boot-ti-staging keeps
# virtual/bootloader, the u-boot package name and the plain deploy names.
PROVIDES = ""
# No packages either: nothing installs this on a target, and u-boot-ti.inc's
# PKG renames (u-boot, u-boot-dev, u-boot-dbg) would collide with the
# production recipe's packages. Only do_deploy's output is wanted.
inherit nopackages
PACKAGECONFIG[ap-trusted-rom] = "BL1=${S}/prebuilt-images/am62lxx-evm/bl1.bin,,trusted-firmware-a"
PACKAGECONFIG[atf] = "BL31=${S}/prebuilt-images/am62lxx-evm/bl31.bin,,trusted-firmware-a"
PACKAGECONFIG[optee] = "TEE=${S}/prebuilt-images/am62lxx-evm/bl32.bin,,optee-os"

# Deploy under a directory of its own. The inherited deploy functions
# write tispl.bin, u-boot.img and friends with the same names the
# production recipe uses; two recipes installing the same file names
# into the shared deploy area is an sstate error, and the station must
# never pick the flasher's u-boot.img up as the production one.
VT_FLASHER_DEPLOY_SUBDIR = "vt-flasher"
do_deploy:append() {
    install -d ${DEPLOYDIR}/${VT_FLASHER_DEPLOY_SUBDIR}
    for f in ${DEPLOYDIR}/* ; do
        [ "$(basename $f)" = "${VT_FLASHER_DEPLOY_SUBDIR}" ] && continue
        mv "$f" ${DEPLOYDIR}/${VT_FLASHER_DEPLOY_SUBDIR}/
    done
}
