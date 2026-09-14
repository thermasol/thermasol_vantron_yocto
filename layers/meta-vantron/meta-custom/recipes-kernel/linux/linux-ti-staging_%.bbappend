FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI = "git://git@git.vantrontech.us:8222/linux/ti/6.12/standard/VT-SBC-AM62L/kernel-ti.git;protocol=ssh;branch=${SRCBRANCH}"
SRCBRANCH = "vt-sbc-am62l"
SRCREV = "1e8098a7ccba6f47094e0caf7ff9f0941d436ab4"

KBUILD_DEFCONFIG = "vt_sbc_am62l_defconfig"

# Keep the unused fitImage out of the rootfs (~16 MB).
#
# KERNEL_IMAGETYPES is "Image fitImage" (k3.inc), so kernel.bbclass packages
# both /boot/Image and /boot/fitImage. U-Boot on this board only ever reads
# the former - its bootcmd is "run args_mmc; run loadimage; run loadfdt; run
# run_kern", and loadimage is "load ${devtype} ${bootpart} ${loadaddr}
# ${bootdir}/${bootfile}" with bootfile=Image. The boot_fit branch lives in
# bootcmd_ti_mmc, which this bootcmd bypasses, IMAGE_BOOT_FILES doesn't ship
# a fitImage either, and flash-tools only pushes emmc.wic plus the loader
# blobs. So /boot/fitImage is 16 MB nothing reads.
#
# It arrives by recommendation, not by request: every kernel-module-* package
# RDEPENDS on kernel-${KERNEL_VERSION} (kernel.bbclass, to pin the module ABI
# to the kernel), that package is kernel-base renamed, and kernel-base
# RRECOMMENDS kernel-image-${KERNEL_VERSION}, which RDEPENDS on *both*
# kernel-image-image and kernel-image-fitimage. So pulling in the three
# display modules pulls in the fitImage.
#
# Clearing the recommendation is enough, and it is a ?= in kernel.bbclass so
# a plain assignment from this bbappend (parsed after the class) wins.
# /boot/Image and /boot/dtb are unaffected: they come from
# MACHINE_ESSENTIAL_EXTRA_RDEPENDS ("kernel-image-image" from k3.inc,
# "kernel-devicetree" from poky), which are hard RDEPENDS of
# packagegroup-core-boot. Doing it this way rather than by editing
# KERNEL_IMAGETYPES also means no kernel rebuild - only do_package.
#
# Spell the key exactly as kernel.bbclass does, unexpanded. Writing the
# expanded "RRECOMMENDS:kernel-base" instead creates a *second*, distinct
# variable key: bitbake then resolves the collision in the class's favour
# ("Variable key RRECOMMENDS:${KERNEL_PACKAGE_NAME}-base (...) replaces
# original key RRECOMMENDS:kernel-base ()") and the fitImage comes back.
RRECOMMENDS:${KERNEL_PACKAGE_NAME}-base = ""

do_configure:prepend() {
    cp ${S}/arch/arm64/configs/${KBUILD_DEFCONFIG} ${WORKDIR}/defconfig

    # Defconfig already has CONFIG_LOGO/CONFIG_LOGO_LINUX_CLUT224/
    # CONFIG_DRM_FBDEV_EMULATION enabled, but not the fbcon driver itself
    # (verified on-device: CONFIG_FRAMEBUFFER_CONSOLE is not set) - so the
    # boot logo prerequisites are all present but nothing is actually bound
    # to a VT to draw it. This is what shows the classic penguin logo.
    echo "CONFIG_FRAMEBUFFER_CONSOLE=y" >> ${WORKDIR}/defconfig
}
