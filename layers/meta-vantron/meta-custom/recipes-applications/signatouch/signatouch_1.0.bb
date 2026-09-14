SUMMARY = "SignaTouch (SignaSteam) Qt5 GUI Application"
DESCRIPTION = "SignaTouch control interface for steam, shower, lighting and music"
LICENSE = "CLOSED"

DEPENDS = "qtbase qtbase-native"
# psplash-signatouch, not bare psplash: the splash artwork is compiled into
# the psplash binary, and the base recipe builds one binary per entry in
# SPLASH_IMAGES. Naming the product package here is what pairs this app
# with its own splash - see recipes-core/psplash/psplash_git.bbappend.
RDEPENDS:${PN} += "qtbase qtbase-plugins display-rotation psplash-signatouch"

# The app shells out to these (SettingsScreenModel/WizardModel/
# setuporientationdialog): /sbin/reboot, date, hwclock, and
# set-display-rotation.sh (via "sh -c"). systemd provides reboot, busybox
# provides sh/date/hwclock. nmcli is also called from the Wi-Fi page of the
# setup wizard - NetworkManager is deliberately NOT installed (this image
# uses systemd-networkd, and NM would pull a large stack in), so that one
# page is a no-op. Revisit if field Wi-Fi provisioning is needed.
RDEPENDS:${PN} += "busybox"

# The product repo is still named SignaSteam upstream; signa_olivia is the
# branch carrying the Olivia hardware work (temperature sensor, orientation
# wizard, LCTESV slave support).
SRC_URI = "git://git@github.com/thermasol/SignaSteam.git;protocol=ssh;branch=signa_olivia;name=signasteam;destsuffix=git \
           git://git@github.com/thermasol/ThermaCan.git;protocol=ssh;branch=master;name=thermacan;destsuffix=thermacan-src \
           file://signatouch.service \
          "
SRCREV_signasteam = "048ea4fecd54a570b1c3f37b1249019557fbed30"

# ThermaCan master, shared with the signatouch-ir recipe - one CAN library
# revision across both products.
#
# It has to be master here, not the older 42aea0ad the legacy signasteam
# recipe builds from its vendored copy: the headers in this app's
# thermacan/include are byte-identical to master's, and the app calls
# lctesv_slave_get_errors(), lctesv_slave_is_connected() and
# lctesv_slave_send_heartbeat(), which are among 39 symbols that do not
# exist at 42aea0ad - linking against that older tree fails. Note that
# 25a35d5 ("thermacan update for slave"), where those headers landed, does
# not compile (the process_msg() call site has the wrong argument list);
# 61afea5 is the fix and 9b6342a is master on top of it.
SRCREV_thermacan = "9b6342a45d07bed6017f84c9281f46c326eb554c"
SRCREV_FORMAT = "signasteam_thermacan"

S = "${WORKDIR}/git"

inherit qmake5 systemd

SYSTEMD_SERVICE:${PN} = "signatouch.service"
SYSTEMD_AUTO_ENABLE = "enable"

# The .pri's own build-variant switch (CONFIG(ARM, X86|ARM)), which selects
# -lthermacan over -lthermacan_x86. Not an actual arch check.
EXTRA_QMAKEVARS_PRE += "CONFIG+=ARM"

do_configure:prepend() {
    # SignaSteam-Test is a QtTest harness; it isn't shipped and it drags in
    # testlib. Drop it from the subdirs project.
    sed -i '/SignaSteam-Test/d' ${S}/SignaSteam-Project.pro

    # The repo has build output committed next to the source (root Makefile
    # and .qmake.stash, and under SignaSteam/ the Makefile, the x86 binary
    # itself, moc_predefs.h and the generated moc_/ui_/qrc_ files and .o
    # objects). qmake5 builds out of tree into ${B}, but stale generated
    # sources in ${S} still get picked up through the include paths, so
    # clear them.
    rm -f ${S}/Makefile ${S}/.qmake.stash ${S}/compile_commands.json
    rm -f ${S}/SignaSteam/Makefile ${S}/SignaSteam/SignaSteam \
          ${S}/SignaSteam/moc_predefs.h ${S}/SignaSteam/qrc_resources.cpp
    rm -f ${S}/SignaSteam/*.o ${S}/SignaSteam/moc_*.cpp ${S}/SignaSteam/moc_*.h \
          ${S}/SignaSteam/ui_*.h

    # thermacan/libs ships a prebuilt libthermacan.so (32-bit ARM, from a
    # different board) and libthermacan_x86.so (dev machine). Neither links
    # into this aarch64 target - do_compile:prepend below rebuilds the real
    # one from ThermaCan.git into the same path the .pri searches
    # (-L$$PWD/thermacan/libs/).
    rm -f ${S}/thermacan/libs/libthermacan.so ${S}/thermacan/libs/libthermacan_x86.so
}

do_compile:prepend() {
    # Build libthermacan.so from source rather than via ThermaCan's own
    # Makefile: that Makefile hardcodes its CFLAGS and never references
    # LDFLAGS in its link rule, so the result is missing Yocto's hardening
    # flags and fails the ldflags/GNU_HASH QA check.
    cd ${WORKDIR}/thermacan-src
    for f in thermacan pipe process_msg log; do
        ${CC} ${CFLAGS} -fpic -std=gnu99 -c $f.c -o $f.o
    done
    ${CC} ${LDFLAGS} -shared -pthread -o libthermacan.so thermacan.o pipe.o process_msg.o log.o
    cp libthermacan.so ${S}/thermacan/libs/libthermacan.so
    cd -
}

do_install() {
    install -d ${D}${bindir}
    # qmake5 sets B="${WORKDIR}/build", a shadow build dir separate from S,
    # and do_install's CWD is B. This is a subdirs project, so the app links
    # into ${B}/SignaSteam/, not ${B}/.
    install -m 0755 ${B}/SignaSteam/SignaSteam ${D}${bindir}/

    install -d ${D}${libdir}
    install -m 0755 ${S}/thermacan/libs/libthermacan.so* ${D}${libdir}/

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/signatouch.service ${D}${systemd_system_unitdir}/

    # QSettings writes /opt/ThermaSol/SignaSteam.conf (main.cpp sets the
    # NativeFormat UserScope path to /opt/), and set-display-rotation.sh
    # reads orientationSelected back out of it at every boot.
    install -d ${D}/opt/ThermaSol
}

FILES:${PN} += "${bindir}/SignaSteam"
FILES:${PN} += "${libdir}/libthermacan.so*"
FILES:${PN} += "/opt/ThermaSol"

# Unversioned .so would otherwise be auto-claimed by -dev before
# FILES:${PN} gets a say.
FILES_SOLIBSDEV = ""

INSANE_SKIP:${PN} += "file-rdeps"

# Both install /usr/bin/SignaSteam and /usr/lib/libthermacan.so. signasteam
# is the older recipe that builds the same app from a vendored source copy;
# this one builds it from git. They are alternatives, never both.
RCONFLICTS:${PN} = "signasteam"
