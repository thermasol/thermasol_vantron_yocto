# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "this is vantron vtvdm"
HOMEPAGE = "https://www.vantrontech.com.cn/"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

DEPENDS += "libvtcom"

SRC_URI = " \
    git://git@github.com/harvia-dev/thermasol_vtvdm.git;protocol=ssh;branch=${SRCBRANCH} \
    "
SRCBRANCH = "arm"
SRCREV = "d77fa513c40dd49b94ec2f5b24a973350cf0220f"

PV = "1.0.0"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

inherit cmake
