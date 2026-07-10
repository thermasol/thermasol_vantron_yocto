# Copyright (C) 2021,Vantron - All Rights Reserved

CORE_IMAGE_EXTRA_INSTALL += " \
    libvtcom \
    libvtcom-dev \
    vtvdm \
    packagegroup-tools-bluetooth \
    brcm-patchram-plus \
    linux-firmware-ap6398s \
    hardwatchdog \
    hardwatchdog-script \
    "

WKS_FILE_DEPENDS = " \
    vtlinux-image-userfs \
"
WKS_FILE = "${MACHINE}.wks.in"

LICENSE_FLAGS_ACCEPTED += "commercial"

IMAGE_PREPROCESS_COMMAND:append = "rootfs_versions;"

rootfs_versions () {
    echo ${IMAGE_VERSION} > ${IMAGE_ROOTFS}/etc/versions
    bbnote "rootfs_reproducible: set /etc/versions to ${IMAGE_VERSION}"
}
