#
# This is the bs application recipe
#
#

SUMMARY = "bs"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://COPYING \
           file://NEWS.adoc \
           file://Makefile \
           file://control \
           file://bs.desktop \
           file://bs.c \
           file://bs.adoc \
           file://README \
        "
S = "${WORKDIR}"

DEPENDS += "ncurses"
RDEPENDS_${PN} += "ncurses"

EXTRA_OEMAKE += 'CC="${CC}" CFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS}"'

do_compile() {
        oe_runmake
}
do_install() {
    install -d ${D}${bindir}/myuart-games
    install -m 0755 ${S}/bs ${D}${bindir}/myuart-games/
}
FILES_${PN} += "${bindir}/myuart-games/*"
