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

PACKAGECONFIG[ap-trusted-rom] = "BL1=${S}/prebuilt-images/am62lxx-evm/bl1.bin,,trusted-firmware-a"
PACKAGECONFIG[atf] = "BL31=${S}/prebuilt-images/am62lxx-evm/bl31.bin,,trusted-firmware-a"
PACKAGECONFIG[optee] = "TEE=${S}/prebuilt-images/am62lxx-evm/bl32.bin,,optee-os"
