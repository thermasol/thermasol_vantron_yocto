SUMMARY = "VtLinux weston image."
LICENSE = "MIT"
#LICENSE = "Proprietary"

inherit core-image

IMAGE_LINGUAS = "en-us"

IMAGE_FEATURES += " \
    package-management \
    ssh-server-openssh \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'weston', \
       bb.utils.contains('DISTRO_FEATURES',     'x11', 'x11-base x11-sato', \
                                                       '', d), d)} \
"

CORE_IMAGE_EXTRA_INSTALL += " \
    packagegroup-framework-base \
"

# do_image_wic[depends] += "${INITRD_IMAGE}:do_image_complete"