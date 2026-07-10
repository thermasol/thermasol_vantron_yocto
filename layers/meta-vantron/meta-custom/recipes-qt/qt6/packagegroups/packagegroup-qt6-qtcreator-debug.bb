SUMMARY = "Remote debugging tools for QtCreator integration"
LICENSE = "MIT"

inherit packagegroup

# Override by distro if needed
VIRTUAL-RUNTIME_qtcreator-debug-ssh-daemon ?= "openssh-sshd"

RDEPENDS:${PN} = " \
    gdbserver \
    ${VIRTUAL-RUNTIME_qtcreator-debug-ssh-daemon} \
    openssh-sftp-server \
    qtdeclarative \
"
