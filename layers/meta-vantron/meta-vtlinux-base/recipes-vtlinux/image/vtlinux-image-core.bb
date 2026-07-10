SUMMARY = "VTLinux base image."
LICENSE = "Proprietary"

inherit core-image

IMAGE_FEATURES += "\
    package-management  \
    ssh-server-dropbear \
    "

#
# INSTALL addons
#
CORE_IMAGE_EXTRA_INSTALL += " \
    packagegroup-framework-base    \
    "
