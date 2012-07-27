# bbappend to openembedded-core/meta/recipes-extended/quota/...
# do not generate rc-links
inherit systemd

PR_append = "+r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://rpc.rquotad.service   \
                   file://quota                 \
                 "

# systemd
PACKAGES =+ "${PN}-systemd"

FILES_${PN}-systemd +=  "   ${base_libdir}/systemd          \
                            ${sysconfdir}/default/quota     \
                        "
RDEPENDS_${PN}-systemd += "${PN}"

SYSTEMD_PACKAGES = "${PN}-systemd"
SYSTEMD_SERVICE = "rpc.rquotad.service"

do_install_append () {
  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/rpc.rquotad.service ${D}${base_libdir}/systemd/system/

  install -d ${D}${sysconfdir}/default
  install -m 0644 ${WORKDIR}/quota ${D}${sysconfdir}/default/
}


