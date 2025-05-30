#
# This is the myuart application recipe
#
#

SUMMARY = "myuart application for games"
SECTION = "PETALINUX/apps"
LICENSE = "GPL-3.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-3.0;md5=c79ff39f19dfec6d293b95dea7b07891"
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
