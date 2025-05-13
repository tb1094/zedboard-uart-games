#
# This is the ncurses-minesweeper application recipe
#
#

SUMMARY = "ncurses-minesweeper"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://Makefile \
           file://LICENSE \
           file://src/time.c \
           file://src/draw/options.c \
           file://src/draw/text.h \
           file://src/draw/title.c \
           file://src/draw/about.c \
           file://src/draw/draw.c \
           file://src/draw/pages.h \
           file://src/draw/game.c \
           file://src/draw/text.c \
           file://src/draw/draw.h \
           file://src/draw/help.c \
           file://src/colors.h \
           file://src/time.h \
           file://src/state.h \
           file://src/game/game.h \
           file://src/game/reset.c \
           file://src/game/new.h \
           file://src/game/kaboom.c \
           file://src/game/constrain_movement.c \
           file://src/game/new.c \
           file://src/game/nearest_cells.h \
           file://src/game/nearest_cells.c \
           file://src/game/kaboom.h \
           file://src/game/flag.c \
           file://src/game/open.h \
           file://src/game/constrain_movement.h \
           file://src/game/done.h \
           file://src/game/open.c \
           file://src/game/reset.h \
           file://src/game/flag.h \
           file://src/game/game.c \
           file://src/game/done.c \
           file://src/strings.c \
           file://src/colors.c \
           file://src/strings.h \
           file://src/main.c \
           file://README.md \
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
    install -m 0755 ${S}/bin/minesweeper ${D}${bindir}/myuart-games/
}
FILES_${PN} += "${bindir}/myuart-games/*"
