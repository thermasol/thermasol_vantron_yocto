# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "Framework private"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

PROVIDES = "${PACKAGES}"
PACKAGES = " \
    packagegroup-framework-private         \
    packagegroup-framework-demo          \
    "

RDEPENDS:packagegroup-framework-private = " \
    packagegroup-framework-demo           \
    "

SUMMARY:packagegroup-framework-demo = "bb demo"
RDEPENDS:packagegroup-framework-demo = " \
    hello     \
    "
