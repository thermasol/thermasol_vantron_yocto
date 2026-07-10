POKY_DEFAULT_DISTRO_FEATURES:remove = "${POKY_DEFAULT_DISTRO_FEATURES_IMX_REMOVE}"
POKY_DEFAULT_DISTRO_FEATURES_IMX_REMOVE             ?= "wayland"
POKY_DEFAULT_DISTRO_FEATURES_IMX_REMOVE:mx6-nxp-bsp ?= "wayland vulkan"
POKY_DEFAULT_DISTRO_FEATURES_IMX_REMOVE:mx7-nxp-bsp ?= "wayland vulkan"
DISTRO_FEATURES:remove = "pulseaudio"

POKY_DEFAULT_DISTRO_FEATURES:append = " ${POKY_DEFAULT_DISTRO_FEATURES_IMX_APPEND}"
POKY_DEFAULT_DISTRO_FEATURES_IMX_APPEND             ?= "jailhouse xen"
POKY_DEFAULT_DISTRO_FEATURES_IMX_APPEND:mx6-nxp-bsp ?= ""
POKY_DEFAULT_DISTRO_FEATURES_IMX_APPEND:mx7-nxp-bsp ?= ""

DISTRO_FEATURES:append:mx8-nxp-bsp = " virtualization"
SKIP_META_VIRT_SANITY_CHECK = "1"

SKIP_META_SECURITY_SANITY_CHECK = "1"
SKIP_META_TPM_SANITY_CHECK = "1"

# Set a more generic tuning for code reuse across parts
DEFAULTTUNE:mx8-nxp-bsp    ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx8m-nxp-bsp   ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx8qm-nxp-bsp  ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx8x-nxp-bsp   ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx8ulp-nxp-bsp ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx9-nxp-bsp    ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx91-nxp-bsp  ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx93-nxp-bsp   ?= "armv8a-crc-crypto"

PACKAGECONFIG:append:mx93-nxp-bsp:pn-qtbase = " gles2"
PACKAGECONFIG:remove:mx93-nxp-bsp:pn-qtbase = "no-opengl linuxfb"

# Use virtual-runc as a default container runtime
# runtime: runc, crun, runv, runx
VIRTUAL-RUNTIME_container_runtime = "virtual-runc"