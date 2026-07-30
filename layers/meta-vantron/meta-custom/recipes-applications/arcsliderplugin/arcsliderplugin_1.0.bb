SUMMARY = "ArcSlider Qt5 widget shared library"
DESCRIPTION = "Custom arc-shaped temperature slider widget used by SignaTouch_IR. \
Upstream (qt_widgit_arcslider) is a Qt Designer plugin project so the widget can \
be previewed in Qt Creator; only the ArcSlider widget implementation itself is \
needed on target, so a minimal .pro replaces the upstream one, dropping the \
Designer/IDE integration (arcsliderplugin.cpp/h, QtUiPlugin dependency) that \
isn't usable on the embedded target anyway."
LICENSE = "CLOSED"

SRC_URI = "git://git@github.com/thermasol/qt_widgit_arcslider.git;protocol=ssh;branch=master \
           file://arcsliderplugin.pro \
          "
SRCREV = "cfe071dd1eb848325e3cf925e3daa0d85f8ba1e5"

S = "${WORKDIR}/git"

DEPENDS = "qtbase qtbase-native qtsvg"

inherit qmake5

do_configure:prepend() {
    rm -f ${S}/TsolArcSlider.pro
    cp ${WORKDIR}/arcsliderplugin.pro ${S}/
}

do_install() {
    install -d ${D}${libdir}
    install -m 0755 ${B}/libarcsliderplugin.so ${D}${libdir}/
}

FILES:${PN} += "${libdir}/libarcsliderplugin.so*"

# Unversioned .so would otherwise be auto-claimed by -dev before
# FILES:${PN} gets a say (same fix as libthermacan.so in signasteam).
FILES_SOLIBSDEV = ""

INSANE_SKIP:${PN} += "file-rdeps"
