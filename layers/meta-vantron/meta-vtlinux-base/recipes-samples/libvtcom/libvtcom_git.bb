# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "libvtcom"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "git://git@github.com/harvia-dev/thermasol_libvtcom.git;protocol=ssh;branch=master"
SRCREV = "65e47c46cfda3ae3ff1f12ebf6bb92751e8e8bac"

PV = "1.0.4"

S = "${WORKDIR}/git"

inherit cmake
