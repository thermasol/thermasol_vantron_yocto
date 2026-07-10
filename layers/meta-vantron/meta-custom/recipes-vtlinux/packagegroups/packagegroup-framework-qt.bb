# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "Framework sample qt components"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

PROVIDES = "${PACKAGES}"
PACKAGES = " \
    packagegroup-framework-qt               \
    packagegroup-framework-qt-base          \
    packagegroup-framework-qt-fonts         \
    packagegroup-framework-qt-extra         \
    "

RDEPENDS:packagegroup-framework-qt = " \
    packagegroup-framework-qt-base          \
    packagegroup-framework-qt-fonts         \
    packagegroup-framework-qt-extra         \
    "

SUMMARY:packagegroup-framework-qt-base = "Qt base"
RDEPENDS:packagegroup-framework-qt-base = " \
    qtbase                          \
    liberation-fonts                \
    qtbase-plugins                  \
    qtbase-tools                    \
    \
    qtdeclarative                   \
    qtdeclarative-qmlplugins        \
    qtdeclarative-tools             \
    \
    qtmultimedia                    \
    qtmultimedia-plugins            \
    qtmultimedia-qmlplugins         \
    \
    "

SUMMARY:packagegroup-framework-qt-fonts = "Qt fonts"
RDEPENDS:packagegroup-framework-qt-fonts = " \
    ttf-dejavu-common               \
    ttf-dejavu-sans                 \
    ttf-dejavu-sans-mono            \
    ttf-dejavu-serif                \
    source-han-sans-cn-fonts        \
    "

SUMMARY:packagegroup-framework-qt-extra = "Qt extra"
RDEPENDS:packagegroup-framework-qt-extra = " \
    qt3d                            \
    qt3d-qmlplugins                 \
    \
    qtsvg                           \
    qtsvg-plugins                   \
    \
    qtlocation                      \
    qtlocation-qmlplugins           \
    qtlocation-plugins              \
    \
    qtsensors                       \
    qtserialport                    \
    \
    qtcharts                        \
    qtcoap \
    qtdatavis3d \
    qtconnectivity \
    qtcharts-qmlplugins             \
    \
    qtlocation                      \
    qtlocation-plugins              \
    qtlocation-qmlplugins           \
    \
    qtvirtualkeyboard               \
    qtimageformats \
	qtlottie \
	qtmqtt \
	qtmultimedia \
	qtnetworkauth \
	qtopcua \
	qtquick3d \
	qtquicktimeline \
	qtremoteobjects \
	qtscxml \
	qtserialbus \
	qttools \
	qttranslations \
	qtwebchannel \
	qtwebsockets \
    "

SUMMARY:packagegroup-framework-qt-examples = "Qt examples"
RDEPENDS:packagegroup-framework-qt-examples = " \
    qtbase-examples                 \
"
