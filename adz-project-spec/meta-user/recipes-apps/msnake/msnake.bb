#
# This is the msnake application recipe
#
#

SUMMARY = "msnake"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://dialog.c \
           file://dialog.h \
           file://Makefile \
           file://effects.c \
           file://effects.h \
           file://events.c \
           file://events.h \
           file://fruits.c \
           file://fruits.h \
           file://game.c \
           file://game.h \
           file://glog.c \
           file://glog.h \
           file://highscore.c \
           file://highscore.h \
           file://main.c \
           file://main.h \
           file://snake.c \
           file://snake.h \
           file://game.c \
           file://game.h \
           file://status-display.c \
           file://status-display.h \
           file://time-helpers.c \
           file://time-helpers.h \
           file://types.h \
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
    install -m 0755 ${S}/snake ${D}${bindir}/myuart-games/
}
FILES_${PN} += "${bindir}/myuart-games/*"
