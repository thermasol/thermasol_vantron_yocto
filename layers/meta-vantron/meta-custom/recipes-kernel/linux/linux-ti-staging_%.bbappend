FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI = "git://git@git.vantrontech.us:8222/linux/ti/6.12/standard/VT-SBC-AM62L/kernel-ti.git;protocol=ssh;branch=${SRCBRANCH}"
SRCBRANCH = "vt-sbc-am62l"
SRCREV = "1e8098a7ccba6f47094e0caf7ff9f0941d436ab4"

KBUILD_DEFCONFIG = "vt_sbc_am62l_defconfig"

do_configure:prepend() {
    cp ${S}/arch/arm64/configs/${KBUILD_DEFCONFIG} ${WORKDIR}/defconfig

    # Defconfig already has CONFIG_LOGO/CONFIG_LOGO_LINUX_CLUT224/
    # CONFIG_DRM_FBDEV_EMULATION enabled, but not the fbcon driver itself
    # (verified on-device: CONFIG_FRAMEBUFFER_CONSOLE is not set) - so the
    # boot logo prerequisites are all present but nothing is actually bound
    # to a VT to draw it. This is what shows the classic penguin logo.
    echo "CONFIG_FRAMEBUFFER_CONSOLE=y" >> ${WORKDIR}/defconfig
}
