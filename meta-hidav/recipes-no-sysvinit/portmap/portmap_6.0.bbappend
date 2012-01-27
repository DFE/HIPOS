# do not generate rc-links
PRINC := "${@int(PRINC) + 1}"
INITSCRIPT_PARAMS = "remove"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = " file://portmap.service "
FILES_${PN} += " /lib/systemd/system/portmap.service "

do_install_append () {

  install -d ${D}/lib/systemd/system
  install -m 0644 ${WORKDIR}/portmap.service ${D}/lib/systemd/system

}

pkg_postinst_append() {

  # don't run flash utils on image install
  if [ "x$D" != "x" ]; then
    exit 1	
  fi

  ln -s '/lib/systemd/system/portmap.service' '/etc/systemd/system/multi-user.target.wants/portmap.service'

}

