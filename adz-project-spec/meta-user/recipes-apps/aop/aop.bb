#
# This is the aop application recipe
#
#

SUMMARY = "aop"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://COPYING \
           file://aop-level-09.txt \
           file://aop-level-10.txt \
           file://aop-level-01.txt \
           file://aop-level-07.txt \
           file://aop.c \
           file://Makefile \
           file://empty-level.txt \
           file://aop-level-08.txt \
           file://aop-level-05.txt \
           file://aop-level-03.txt \
           file://aop-level-11.txt \
           file://aop-level-06.txt \
           file://aop-level-04.txt \
           file://aop-level-02.txt \
           file://README \
        "
S = "${WORKDIR}"

DEPENDS += "ncurses"
RDEPENDS_${PN} += "ncurses"

EXTRA_OEMAKE += 'CC="${CC}" CFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS} -lncurses"'

do_compile() {
        oe_runmake
}
do_install() {
    install -d ${D}${bindir}/myuart-games
    install -m 0755 ${S}/aop ${D}${bindir}/myuart-games/
    install -m 0644 ${S}/aop-level-*.txt ${D}${bindir}/myuart-games/
}
FILES_${PN} += "${bindir}/myuart-games/*"
