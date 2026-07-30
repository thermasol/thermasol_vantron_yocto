SUMMARY = "SignaSteam Qt5 GUI Application"
DESCRIPTION = "SignaSteam control interface for steam, shower, lighting and music"
LICENSE = "CLOSED"

DEPENDS = "qtbase qtbase-native"
RDEPENDS:${PN} += "display-rotation"

SRC_URI = "file://SignaSteam-Project.pro \
           file://SignaSteam-src.pri \
           file://SignaSteam \
           file://thermacan \
           file://signasteam.service \
          "

S = "${WORKDIR}"

inherit qmake5 systemd

SYSTEMD_SERVICE:${PN} = "signasteam.service"
SYSTEMD_AUTO_ENABLE = "enable"

# Set build configuration for ARM
EXTRA_QMAKEVARS_PRE += "CONFIG+=ARM"

# Point to thermacan library
EXTRA_QMAKEVARS_PRE += "LIBS+=-L${S}/thermacan"

do_configure:prepend() {
    # Remove SignaSteam-Test from the project file since we don't need tests
    sed -i '/SignaSteam-Test/d' ${S}/SignaSteam-Project.pro

    # Ensure thermacan library path is available
    export LIBRARY_PATH="${S}/thermacan:${LIBRARY_PATH}"
}

do_compile:prepend() {
    # thermacan/ only ships a prebuilt libthermacan_x86.so (built on an x86 dev
    # machine per its own HOWTOBUILD/copy_lib.sh) — there's no aarch64 build,
    # so the final link against -lthermacan fails. Build it ourselves instead
    # of via its own Makefile: that Makefile hardcodes its own CFLAGS and
    # doesn't reference LDFLAGS in its link rule at all, so going through it
    # (even with CC overridden) produces a .so missing Yocto's hardening
    # flags entirely (fails the ldflags/GNU_HASH QA check).
    cd ${S}/thermacan
    for f in thermacan pipe process_msg log; do
        ${CC} ${CFLAGS} -fpic -std=gnu99 -c $f.c -o $f.o
    done
    ${CC} ${LDFLAGS} -shared -pthread -o libthermacan.so thermacan.o pipe.o process_msg.o log.o
    cd -
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 SignaSteam/SignaSteam ${D}${bindir}/

    # Install thermacan library if it exists. qmake5 sets B="${WORKDIR}/build"
    # (an out-of-tree shadow build dir separate from S="${WORKDIR}"), and
    # do_install's CWD is ${B}, not ${S} — so this must use an absolute path
    # to where do_compile actually built libthermacan.so (${S}/thermacan/),
    # not the bare relative "thermacan/libthermacan.so" (which silently
    # resolved against ${B} and never matched, so the library never made it
    # into the package).
    if [ -f ${S}/thermacan/libthermacan.so ]; then
        install -d ${D}${libdir}
        install -m 0755 ${S}/thermacan/libthermacan.so* ${D}${libdir}/
    fi

    # Install systemd service
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/signasteam.service ${D}${systemd_system_unitdir}/
}

FILES:${PN} += "${bindir}/SignaSteam"
FILES:${PN} += "${libdir}/libthermacan.so*"

# libthermacan.so has no version suffix, so Yocto's automatic packaging
# treats it as a dev-only symlink and claims it for ${PN}-dev before our
# FILES:${PN} assignment above ever gets a say (packages are matched in
# PACKAGES order, dev before the main package). Clearing this stops that
# auto-claim so it actually ships in the main runtime package.
FILES_SOLIBSDEV = ""

# Allow shipping the library in the same package
INSANE_SKIP:${PN} += "file-rdeps"

RDEPENDS:${PN} += "qtbase qtbase-plugins"

# Boot splash (psplash) shown from early boot until we're ready to draw;
# signasteam.service sends it the QUIT handoff signal on startup.
RDEPENDS:${PN} += "psplash"
