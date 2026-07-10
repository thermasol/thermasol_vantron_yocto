# Copyright (C) 2021,Vantron - All Rights Reserved
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"
#PACKAGECONFIG:append   = " no-opengl linuxfb sql-sqlite"
PACKAGECONFIG:remove   = " eglfs "
PACKAGECONFIG:remove:class-native = " linuxfb sql-sqlite"
PACKAGECONFIG:remove:class-nativesdk = " linuxfb sql-sqlite"
SRC_URI += "\
    file://0001-feat-mali-compile.patch \
    file://0001-feat-egl-compile.patch \
    file://0001-feat-eglfs-compile.patch \
"

PACKAGECONFIG_X11 += " ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'xlib', '', d)} "

PACKAGECONFIG[xlib] = "-DFEATURE_xlib=ON,-DFEATURE_xlib=OFF,libx11"

#do_configure:prepend() {
#    rm -f ${STAGING_LIBDIR}/libEGL.so.1
#    ln -sf ${STAGING_LIBDIR}/libmali.so.1.9.0 ${STAGING_LIBDIR}/libEGL.#so.1
#}