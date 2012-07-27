# bbappend to openembedded-core/meta/recipes-extended/rpcbind/...
# do not generate rc-links
inherit systemd

PR_append = "+r1"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://rpcbind.service   \
                   file://rpcbind           \
                 "

# systemd
PACKAGES =+ "${PN}-systemd"

FILES_${PN}-systemd +=  "   ${base_libdir}/systemd          \
                            ${sysconfdir}/default/rpcbind   \
                        "
RDEPENDS_${PN}-systemd += "${PN}"

SYSTEMD_PACKAGES = "${PN}-systemd"
SYSTEMD_SERVICE = "rpcbind.service"

do_install_append () {
  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/rpcbind.service ${D}${base_libdir}/systemd/system/

  install -d ${D}${sysconfdir}/default
  install -m 0644 ${WORKDIR}/rpcbind ${D}${sysconfdir}/default/
}


