SRC_URI = "git://git@git.vantrontech.us:8222/linux/ti/6.12/standard/VT-SBC-AM62L/uboot-ti.git;protocol=ssh;branch=${SRCBRANCH}"

SRCBRANCH = "vt-sbc-am62l"
SRCREV = "bba251a2332e3a96157ae247020f6a7ff264ce25"

PACKAGECONFIG[ap-trusted-rom] = "BL1=${S}/prebuilt-images/am62lxx-evm/bl1.bin,,trusted-firmware-a"
PACKAGECONFIG[atf] = "BL31=${S}/prebuilt-images/am62lxx-evm/bl31.bin,,trusted-firmware-a"
PACKAGECONFIG[optee] = "TEE=${S}/prebuilt-images/am62lxx-evm/bl32.bin,,optee-os"
