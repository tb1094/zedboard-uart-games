SUMMARY = "Install startup script"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://startup \
           file://vt100-patched.src \
        "
RDEPENDS_${PN}_append += "bash"
S = "${WORKDIR}"

INITSCRIPT_NAME = "startup"
INITSCRIPT_PARAMS = "start 99 S ."
inherit update-rc.d

do_install() {
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${S}/startup ${D}${sysconfdir}/init.d

    # copy vt100-patched.src to /home/root/
    install -d ${D}/home/root
    install -m 0644 ${WORKDIR}/vt100-patched.src ${D}/home/root/vt100-patched.src
}
FILES_${PN} += "${sysconfdir}/*"
FILES_${PN} += "/home/root/vt100-patched.src"
