# Copyright (C) 2021,Vantron - All Rights Reserved

SUMMARY = "userfs Image"
LICENSE = "MIT"

inherit core-image

IMAGE_FSTYPES:remove = "wic"

IMAGE_NAME_SUFFIX = ".userfs"

IMAGE_PARTITION_MOUNTPOINT = "/usr/local"

EXTRA_IMAGECMD_ext4 = "-i 4096 -L userfs -O ^metadata_csum,^dir_index"

# Reset image feature
IMAGE_FEATURE = ""

# Define to null ROOTFS_MAXSIZE
IMAGE_ROOTFS_MAXSIZE = ""

# Reset PACKAGE_INSTALL to avoid getting installed packages added in machine through IMAGE_INSTALL:append:
PACKAGE_INSTALL = ""

# Reset LINGUAS_INSTALL to avoid getting installed any locale-base package
LINGUAS_INSTALL = ""
IMAGE_LINGUAS = ""

# Add demo application described on specific packagegroup
PACKAGE_INSTALL += ""

# Reset LDCONFIG to avoid runing ldconfig on image.
LDCONFIGDEPEND = ""

# Remove from IMAGE_PREPROCESS_COMMAND useless buildinfo
IMAGE_PREPROCESS_COMMAND:remove = "buildinfo;"
# Remove from IMAGE_PREPROCESS_COMMAND the prelink_image as it could be run after
# we clean rootfs folder leading to cp error if '/etc/' folder is missing:
#   cp: cannot create regular file
#   No such file or directory
IMAGE_PREPROCESS_COMMAND:remove = "prelink_image;"

IMAGE_PREPROCESS_COMMAND:append = "reformat_rootfs;"

# Cleanup rootfs newly created
reformat_rootfs() {
    if [ -d ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT} ]; then
        TARGETROOTFS=${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}
        while [ "${TARGETROOTFS}" != "${IMAGE_ROOTFS}" ]
        do
            find $(dirname ${TARGETROOTFS})/ -mindepth 1 ! -regex "^${TARGETROOTFS}\(/.*\)?" -delete
            TARGETROOTFS=$(dirname ${TARGETROOTFS})
        done
        bbnote ">>> Move ${IMAGE_PARTITION_MOUNTPOINT} contents to ${IMAGE_ROOTFS}"
        mv ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/* ${IMAGE_ROOTFS}/
        bbnote ">>> Remove remaining ${IMAGE_PARTITION_MOUNTPOINT} folder"
        # Remove empty boot folder
        TARGETROOTFS=${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}
        while [ "${TARGETROOTFS}" != "${IMAGE_ROOTFS}" ]
        do
            bbnote ">>> Delete ${TARGETROOTFS}"
            rm -rf ${TARGETROOTFS}/
            TARGETROOTFS=$(dirname ${TARGETROOTFS})
        done
    else
        bbwarn "${IMAGE_PARTITION_MOUNTPOINT} folder not available in rootfs folder, no reformat done..."
    fi
}
