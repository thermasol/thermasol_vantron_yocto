SUMMARY = "this is hello world program"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " file://hello.c \
            file://Makefile \
          "

S = "${WORKDIR}"

do_install() {
    install -d ${D}${prefix}/bin/
    install -m 0755 ${B}/hello ${D}${prefix}/bin/
}

# No GNU_HASH in the elf binary
TARGET_CC_ARCH += "${LDFLAGS}"

FILES:${PN} += "${prefix}/bin/"
