FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
# Mirrored to GitHub because git.vantrontech.us is only reachable with Vantron
# credentials, which not everyone on this team has. The mirror tracks the
# Vantron branch verbatim -- SRCREV below is Vantron's own commit, unmodified.
SRC_URI = "git://git@github.com/harvia-dev/thermasol_kernel-ti-am62l.git;protocol=ssh;branch=${SRCBRANCH}"
SRCBRANCH = "vt-sbc-am62l"
SRCREV = "1e8098a7ccba6f47094e0caf7ff9f0941d436ab4"

# Panel backlight comes up at 100/255 from the kernel instead of full scale.
# Every rootfs writer (set-display-rotation.sh, the SignaSteam app) uses the
# same level, so 100 is the product's effective maximum. See the patch
# header for why this is not a max_brightness cap.
SRC_URI += "file://vt-sbc-am62l-backlight-default-brightness-100.patch"

# The SD slot is empty in the product and has no card-detect line, so
# Linux kept its rail on and polled it every second forever. Off.
SRC_URI += "file://vt-sbc-am62l-disable-unused-sd-slot.patch"

# The AP6256 Wi-Fi/BT module has no driver in the product image but its
# power was still forced on from SPL by the DT (regulator-always-on /
# boot-on plus a pull-up on the enable pad). Leave it off. Mirrored in
# U-Boot's DT copy (recipes-bsp/u-boot); both patches have to go for
# KIOSK_WIFI / KIOSK_BLUETOOTH = "1" builds.
SRC_URI += "file://vt-sbc-am62l-leave-the-wifi-bt-module-unpowered.patch"

# A53 cpufreq through the SCMI clock: a 600 MHz - 1.25 GHz OPP table on the
# cluster clock, which cpufreq-dt picks up. Stock has no OPP table and the
# cores sit at 1.25 GHz forever. With cpufreq.default_governor=ondemand in
# U-Boot's optargs the boot still runs flat out (ondemand goes to the top
# OPP under load) and the idle app drops to 600 MHz. Verified on the bench
# board: Vantron's DM firmware accepts the rate change.
SRC_URI += "file://vt-sbc-am62l-a53-cpufreq-via-scmi.patch"

# The 5 V long-cable reboot loop (stock image loops too). The probe of the
# gpio_5v_en regulator closed the switched 5 V rail at ~0.75 s: ~0.75 A for
# ~1 ms, board-side voltage to 3.8 V, input cut-out, cold restart every 4 s.
# The patch leaves that rail and the unused SD IO rail off through the boot
# and adds a regulator-output node so the 5 V rail can be switched from
# userspace (`rail-5v on`, recipes-core/rail-5v) once the system is up.
# Needs CONFIG_REGULATOR_USERSPACE_CONSUMER (do_configure below).
SRC_URI += "file://vt-sbc-am62l-5v-rail-off-at-boot.patch"

# The other boot-time step that trips the 5 V feed: the backlight boost's
# ~0.5 A cold inrush, fired by pwm-backlight at display-on on top of the
# panel's own surge. The driver patch gives regulator-fixed an optional
# ramped (PWM) enable; the DT patch applies it to the backlight boost and
# brings the panel up dark, so backlight-boot.service can raise it to the
# product level a moment later, through the ramp, when nothing else is
# switching. The 100/255 default-brightness patch above is superseded by
# that: userspace owns the level now.
SRC_URI += "file://0003-regulator-fixed-optional-GPIO-soft-start.patch"
SRC_URI += "file://vt-sbc-am62l-backlight-soft-start.patch"

# Brownout diagnostics: the PMIC driver prints at probe, on the serial
# console, whether the PMIC lost its 5 V input since the flags were last
# cleared, and clears them. One line per boot; tells a cable brownout from
# any other reset.
SRC_URI += "file://0001-mfd-tps65219-log-and-clear-POWER_UP_STATUS-at-probe.patch"

# Real bug in TI's 6.12 tps65219 regulator driver: the per-IRQ data arrays
# are allocated with the IRQ *count* as the byte size, so the probe writes
# ~800 bytes past a 36-byte object on every boot. Seen on the bench as
# random oopses right after it (omap_i2c, at24/nvmem) that left the unit
# hung dark. Same fix as upstream 2025.
SRC_URI += "file://0002-regulator-tps65219-fix-irq_data-allocation-size.patch"

# ThermaSol boot logo. fbcon draws the compiled-in logo the moment the
# framebuffer registers (~1.5 s after kernel start now that the display
# chain is built in), which makes it the first thing on the panel. The file
# is the kernel's own logo_linux_clut224.ppm slot, replaced wholesale: an
# ASCII P3 PPM, <= 224 colours, generated from the wordmark
# tsol_logo_305x27.png flattened onto black (fbcon has no alpha; the
# console background is black) and padded to 308x27 so it is pixel-identical
# to psplash's copy, which needs a width that is a multiple of 4. psplash then draws the same artwork at the
# same centred position (see recipes-core/psplash), so the handoff is
# invisible. The logo is drawn in the panel's native (portrait) orientation
# and does not follow the app's orientation setting - on a landscape-mounted
# unit it reads sideways for the ~7 s before the app starts.
#
# Placement and count come from the kernel command line, which U-Boot
# passes via optargs (recipes-bsp/u-boot): fbcon=logo-pos:center centres
# it, logo-count:1 stops fbcon drawing one copy per CPU (two on this SoC).
SRC_URI += "file://logo_linux_clut224.ppm"

# Second product on the same kernel. The IR product must not show the
# ThermaSol mark, so a second logo (Almost Heaven wordmark, white on
# black, generated from recipes-core/psplash/files/signatouch-ir.png -
# keep the two in step) is compiled in next to the default and picked at
# boot with logo.select=almostheaven, which the IR image passes through
# U-Boot via /boot/image.env (recipes-bsp/u-boot, vtlinux-image-
# signatouch-ir). The patch adds the parameter and the Kconfig entry.
SRC_URI += "file://0004-video-logo-select-a-logo-by-name-from-the-command-line.patch"
SRC_URI += "file://logo_almostheaven_clut224.ppm"

# Copied into the source tree from do_configure below (do_patch is a python
# task in patch.bbclass, so it cannot take a shell append). The logo .c is
# generated from the .ppm at compile time, so configure is early enough.

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

    # Display chain built in, not as modules. Measured on-device: with
    # cdns-dsi, cdns-dphy, the ST7701 panel and pwm-bl as modules, fb0 did
    # not exist until ~13.9 s after kernel start. udev has to load them
    # from the rootfs, and udev-extraconf's automount/autonet RUN scripts
    # were tying up every udev worker from 7.3 s to 11.2 s, so nothing got
    # loaded until then and tidss then sat in deferred probe until the
    # 10 s deferred_probe_timeout released it. Built in, the panel probes
    # inside the kernel at ~2 s: the boot logo shows immediately, psplash
    # succeeds on its first attempt instead of crash-looping on a missing
    # /dev/fb0, and the app no longer aborts on a missing /dev/dri/card0.
    #
    # BACKLIGHT_CLASS_DEVICE has to be =y as well: the defconfig already
    # says BACKLIGHT_PWM=y and DRM_PANEL_SITRONIX_ST7701=y, but both depend
    # on the class device, which was =m, so Kconfig demoted them to =m.
    # KIOSK_MODULES_DISPLAY in vtlinux-image-kiosk.inc is emptied to match -
    # the kernel-module-* packages no longer exist.
    echo "CONFIG_BACKLIGHT_CLASS_DEVICE=y" >> ${WORKDIR}/defconfig
    echo "CONFIG_BACKLIGHT_PWM=y" >> ${WORKDIR}/defconfig
    echo "CONFIG_DRM_CDNS_DSI=y" >> ${WORKDIR}/defconfig
    echo "CONFIG_PHY_CADENCE_DPHY=y" >> ${WORKDIR}/defconfig
    echo "CONFIG_DRM_PANEL_SITRONIX_ST7701=y" >> ${WORKDIR}/defconfig

    # Equivalent of "quiet" on the kernel command line, which U-Boot's
    # bootcmd on this board gives us no hook for (it never loads
    # uEnv.txt, and arm64 has no CMDLINE_EXTEND). The kernel printed 22 KB
    # (328 lines) to the 115200-baud serial console before reaching init:
    # about 1.9 s of a 2.9 s kernel boot. Warnings and errors still print.
    echo "CONFIG_CONSOLE_LOGLEVEL_DEFAULT=4" >> ${WORKDIR}/defconfig

    # Userspace switch for the 5 V rail (rail-5v regulator-output node in
    # vt-sbc-am62l-5v-rail-off-at-boot.patch).
    echo "CONFIG_REGULATOR_USERSPACE_CONSUMER=y" >> ${WORKDIR}/defconfig

    # ThermaSol boot logo - see the SRC_URI note above.
    cp ${WORKDIR}/logo_linux_clut224.ppm ${S}/drivers/video/logo/logo_linux_clut224.ppm

    # Almost Heaven boot logo for the IR product (logo.select=almostheaven).
    cp ${WORKDIR}/logo_almostheaven_clut224.ppm ${S}/drivers/video/logo/logo_almostheaven_clut224.ppm
    echo "CONFIG_LOGO_ALMOSTHEAVEN_CLUT224=y" >> ${WORKDIR}/defconfig
}
