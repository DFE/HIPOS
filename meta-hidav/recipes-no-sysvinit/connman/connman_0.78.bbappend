# do not generate rc-links
PRINC := "${@int(PRINC) + 4}"
INITSCRIPT_NAME = "-f connman"
INITSCRIPT_PARAMS = "remove"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SYSTEMD_SERVICE_${PN}-systemd = "connman.service"

SRC_URI += "file://connman.service"

do_install_append() {
    install -d ${D}${base_libdir}/systemd/system
    install -m 644 ${WORKDIR}/connman.service ${D}${base_libdir}/systemd/system
}

pkg_postinst_append() {

  rm -f $D/etc/init.d/connman

}

