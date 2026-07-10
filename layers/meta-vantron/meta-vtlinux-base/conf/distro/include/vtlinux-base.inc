require conf/distro/poky.conf

DISTRO_VERSION = "6.6-scarthgap"
DISTROOVERRIDES = "vtlinux"

MAINTAINER = "Kerker <yf.zhou@vantrontech.com>"

# Use systemd as default init manager
POKY_INIT_MANAGER = "systemd"

PACKAGECONFIG:append:pn-gstreamer1.0-plugins-ugly = " x264"

PACKAGECONFIG:append:pn-perf = " jevents"

PACKAGECONFIG:append:pn-pulseaudio = " autospawn-for-root"

PACKAGECONFIG:append:class-target:pn-qtquick3d = " examples"
PACKAGECONFIG:remove:class-target:pn-qtquick3d = "openxr"

PACKAGECONFIG:append:class-target:pn-qtwebengine = " examples"

PACKAGECONFIG:append:pn-systemd = " unmanaged-network"

#################################################################
# Don't include the DATE variable in the sstate package signatures
DISTRO_VERSION[vardepsexclude] = "DATE"

DISTRO_CODENAME = "scarthgap"

# Warning: TARGET_VENDOR does not contains a valid OS/ARCH name like : linux, arm
TARGET_VENDOR = "-vtlinux"

# Append distro name to each image name
IMAGE_BASENAME:append = "-${DISTRO}"

# Warning: SDK_VENDOR does not contains a valid OS/ARCH name like : linux, arm
SDK_VENDOR = "-vtlinuxsdk"
SDK_VERSION := "${@'${DISTRO_VERSION}'.replace('snapshot-${DATE}','snapshot')}"

# Don't include the DATE variable in the sstate package signatures
SDK_VERSION[vardepsexclude] = "DATE"

# Add image name for generated SDK and set default SDK install folder
SDK_NAME = "${DISTRO}-${SDK_ARCH}"
SDKPATHINSTALL = "/opt/${DISTRO}/${MACHINE}/${SDK_VERSION}"

# Don't include the DATE variable in the sstate package signatures
SDKPATHINSTALL[vardepsexclude] = "DATE"

IMAGE_LINGUAS = "en-us en-gb"

