#
# This is the myuart application recipe
#
#

SUMMARY = "myuart application for games"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://myuart_run.c \
           file://gameselection.c \
           file://Makefile \
        "
S = "${WORKDIR}"

DEPENDS += "ncurses"
RDEPENDS_${PN} += "ncurses"

do_compile() {
    oe_runmake
}
do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/myuart_run ${D}${bindir}
    install -m 0755 ${S}/gameselection ${D}${bindir}
}
FILES_${PN} += "${bindir}/myuart_run ${bindir}/gameselection"

