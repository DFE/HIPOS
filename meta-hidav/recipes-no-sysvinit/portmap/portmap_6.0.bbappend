# do not generate rc-links
PRINC := "${@int(PRINC) + 6}"
INITSCRIPT_NAME = "-f portmap"
INITSCRIPT_PARAMS = "remove"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = " file://portmap.service "
FILES_${PN}-systemd += " /lib/systemd/system/* "
RDEPENDS_${PN}-systemd += " ${PN} systemd "

PACKAGES += "${PN}-systemd"

do_install_append () {

  install -d ${D}/lib/systemd/system
  install -m 0644 ${WORKDIR}/portmap.service ${D}/lib/systemd/system
  install -d ${D}${base_libdir}/systemd/system/multi-user.target.wants
  cd '${D}${base_libdir}/systemd/system/multi-user.target.wants'
  ln -s '../portmap.service' 'portmap.service'
  cd -
  
}

pkg_postinst_append() {

  rm -f $D/etc/init.d/portmap

}

