# Copyright (C) 2021,Vantron - All Rights Reserved
#
# meta-qt5 and meta-qt6 both name their qtbase recipe file "qtbase_git.bb", so
# a "qtbase_%.bbappend" here matches meta-qt5's qtbase too, not just meta-qt6's.
# Everything below is Qt6/Mali-specific (and PACKAGECONFIG:remove eglfs would
# break Qt5 EGLFS apps like signasteam), so gate it to only apply when the base
# recipe being appended is actually meta-qt6's.
# Note: FILE here refers to this .bbappend's own path during := (immediate)
# expansion, not the base recipe's, so it can't be conditioned the same way as
# the lazily-expanded assignments below. Leaving it unconditional is harmless —
# it just adds a search path that Qt5's SRC_URI (gated below) never references.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"
#PACKAGECONFIG:append   = " no-opengl linuxfb sql-sqlite"
PACKAGECONFIG:remove   = "${@'eglfs' if 'meta-qt6' in d.getVar('FILE') else ''}"
PACKAGECONFIG:remove:class-native = "${@'linuxfb sql-sqlite' if 'meta-qt6' in d.getVar('FILE') else ''}"
PACKAGECONFIG:remove:class-nativesdk = "${@'linuxfb sql-sqlite' if 'meta-qt6' in d.getVar('FILE') else ''}"
SRC_URI += "${@'file://0001-feat-mali-compile.patch file://0001-feat-egl-compile.patch file://0001-feat-eglfs-compile.patch' if 'meta-qt6' in d.getVar('FILE') else ''}"

PACKAGECONFIG_X11 += " ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'xlib', '', d)} "

PACKAGECONFIG[xlib] = "-DFEATURE_xlib=ON,-DFEATURE_xlib=OFF,libx11"

#do_configure:prepend() {
#    rm -f ${STAGING_LIBDIR}/libEGL.so.1
#    ln -sf ${STAGING_LIBDIR}/libmali.so.1.9.0 ${STAGING_LIBDIR}/libEGL.#so.1
#}