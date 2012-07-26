# bbapped to meta-openembedded/meta-oe/recipes-connectivity/samba/samba_3.5.6.bb

inherit systemd

PR_append = "+r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://samba.service "

# systemd
PACKAGES =+ "${PN}-systemd"

FILES_${PN}-systemd += "${base_libdir}/systemd"

RDEPENDS_${PN}-systemd += "${PN}"

SYSTEMD_PACKAGES = "${PN}-systemd"

SYSTEMD_SERVICE = "samba.service "

do_install_append () {
  install -d ${D}${base_libdir}/systemd/system

  for service in $SYSTEMD_SERVICE; do
    install -m 0644 ${WORKDIR}/$service ${D}${base_libdir}/systemd/system/
  done
}

