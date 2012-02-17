# do not generate rc-links
inherit systemd

PR_append = "+r4"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://portmap.service "

# systemd
PACKAGES =+ "${PN}-systemd"

FILES_${PN}-systemd += "${base_libdir}/systemd"
RDEPENDS_${PN}-systemd += "${PN}"

SYSTEMD_PACKAGES = "${PN}-systemd"
SYSTEMD_SERVICE = "portmap.service"

FILES_${PN} = " ${base_sbindir}/portmap \
		${base_libdir} \
"

do_install_append () {

  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/portmap.service ${D}${base_libdir}/systemd/system
}

pkg_postrm_${PN}-systemd_prepend() {
	exit 0
}

