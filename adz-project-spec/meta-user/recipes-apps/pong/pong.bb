#
# This is the pong application recipe
#
#

SUMMARY = "pong"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://Makefile \
           file://LICENSE \
           file://pong.c \
           file://README.md \
        "
S = "${WORKDIR}"

DEPENDS += "ncurses"
RDEPENDS_${PN} += "ncurses"

EXTRA_OEMAKE += 'CC="${CC}" CFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS} -lm -lncurses"'

do_compile() {
        oe_runmake
}
do_install() {
    install -d ${D}${bindir}/myuart-games
    install -m 0755 ${S}/pong ${D}${bindir}/myuart-games/
}
FILES_${PN} += "${bindir}/myuart-games/*"
