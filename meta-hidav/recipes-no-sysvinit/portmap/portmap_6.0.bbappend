# do not generate rc-links
inherit systemd

EXTRA_OECONF += "--with-systemdunitdir=${base_libdir}/systemd/system/"

PRINC := "${@int(PRINC) + 7}"
INITSCRIPT_NAME = "-f portmap"
INITSCRIPT_PARAMS = "remove"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = " file://portmap.service "
FILES_${PN}-systemd += "${base_libdir}/systemd"
RDEPENDS_${PN}-systemd += "${PN}"

PACKAGES =+ "${PN}-systemd"
SYSTEMD_PACKAGES = "${PN}-systemd"
SYSTEMD_SERVICE_${PN}-systemd = "portmap.service"

do_install_append () {

  install -d ${D}/lib/systemd/system
  install -m 0644 ${WORKDIR}/portmap.service ${D}/lib/systemd/system
}

pkg_postinst_append() {

  rm -f $D/etc/init.d/portmap

}

