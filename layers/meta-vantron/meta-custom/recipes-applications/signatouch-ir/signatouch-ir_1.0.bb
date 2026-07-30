SUMMARY = "SignaTouch IR Qt5 GUI Application"
DESCRIPTION = "SignaTouch IR control interface for infrared sauna"
LICENSE = "CLOSED"

DEPENDS = "qtbase qtbase-native qtsvg arcsliderplugin"
RDEPENDS:${PN} += "qtbase qtbase-plugins qtsvg arcsliderplugin display-rotation psplash"

SRC_URI = "git://git@github.com/thermasol/SignaTouch_IR.git;protocol=ssh;branch=olivia_yocto;name=signatouchir;destsuffix=git \
           git://git@github.com/thermasol/ThermaCan.git;protocol=ssh;branch=master;name=thermacan;destsuffix=thermacan-src \
           file://signatouch-ir.service \
          "
# olivia_yocto is a descendant of the master commit this was previously
# pinned to (592f3e6) - it's a superset, not a divergent branch.
SRCREV_signatouchir = "e77538a44214499a70d7db67be881de1f084e494"
# Pinned to the same ThermaCan commit already proven working (cross-compile
# quirks and all) in the signasteam recipe, for consistency between products.
SRCREV_thermacan = "42aea0ad9eb19eeafe2af6f32bf4dea460815e0d"
SRCREV_FORMAT = "signatouchir_thermacan"

S = "${WORKDIR}/git"

inherit qmake5 systemd

SYSTEMD_SERVICE:${PN} = "signatouch-ir.service"
SYSTEMD_AUTO_ENABLE = "enable"

# Set build configuration for ARM (this is just the upstream .pro's own
# build-variant switch name, not an actual arch check - matches the same
# CONFIG(ARM, X86|ARM) pattern already used successfully in signasteam).
EXTRA_QMAKEVARS_PRE += "CONFIG+=ARM"

do_configure:prepend() {
    # The upstream repo has build artifacts committed alongside source
    # (Makefile, .qmake.stash, moc_predefs.h, the prebuilt binary itself,
    # generated qrc_sauna_assets.cpp) - clear them so qmake/make don't get
    # confused by stale output from a different architecture.
    rm -f ${S}/Makefile ${S}/.qmake.stash ${S}/moc_predefs.h \
          ${S}/SignaTouch_IR ${S}/SignaTouch_IR.debug ${S}/qrc_sauna_assets.cpp

    # include/lib{thermacan,arcsliderplugin}*.so are prebuilt for x86 (dev
    # machine) or 32-bit ARM (an older, different board) - neither links
    # into this aarch64 target. Remove them: do_compile:prepend rebuilds
    # libthermacan.so from source into the same path (matching the .pro's
    # existing -L$$PWD/include/ search path), and libarcsliderplugin.so
    # comes from the arcsliderplugin recipe via the normal sysroot search
    # path instead (DEPENDS above stages it there at build time).
    rm -f ${S}/include/libthermacan.so ${S}/include/libthermacan_x86.so
    rm -f ${S}/include/libarcsliderplugin.so ${S}/include/libarcsliderplugin_x86.so
}

do_compile:prepend() {
    cd ${WORKDIR}/thermacan-src
    for f in thermacan pipe process_msg log; do
        ${CC} ${CFLAGS} -fpic -std=gnu99 -c $f.c -o $f.o
    done
    ${CC} ${LDFLAGS} -shared -pthread -o libthermacan.so thermacan.o pipe.o process_msg.o log.o
    cp libthermacan.so ${S}/include/libthermacan.so
    cd -
}

do_install() {
    install -d ${D}${bindir}
    # qmake5's B ("${WORKDIR}/build") is a shadow build dir separate from S
    # - do_install's CWD is B, not S, so the built binary lives here, not
    # under S (the same qmake5/thermacan lesson learned in signasteam).
    install -m 0755 ${B}/SignaTouch_IR ${D}${bindir}/

    install -d ${D}${libdir}
    install -m 0755 ${S}/include/libthermacan.so* ${D}${libdir}/

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/signatouch-ir.service ${D}${systemd_system_unitdir}/
}

FILES:${PN} += "${bindir}/SignaTouch_IR"
FILES:${PN} += "${libdir}/libthermacan.so*"

# Unversioned .so would otherwise be auto-claimed by -dev before
# FILES:${PN} gets a say.
FILES_SOLIBSDEV = ""

INSANE_SKIP:${PN} += "file-rdeps"
