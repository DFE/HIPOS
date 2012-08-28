FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
PR_append = "+r3"
#replaces files/lighttpd.conf automatically

FILES_${PN} +=  "   ${sysconfdir}/default/lighttpd \
                "

do_install_append() {
  install -d ${D}${sysconfdir}/default
  install -m 0644 ${WORKDIR}/lighttpd ${D}${sysconfdir}/default/
}
