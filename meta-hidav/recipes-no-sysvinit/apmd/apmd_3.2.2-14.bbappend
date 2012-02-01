# do not generate rc-links
PRINC := "${@int(PRINC) + 2}"
INITSCRIPT_NAME = "-f apmd"
INITSCRIPT_PARAMS = "remove"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = " file://apmd.service "
FILES_${PN} += " /lib/systemd/system/apmd.service "

install_append () {

  install -d ${D}/lib/systemd/system
  install -m 0644 ${WORKDIR}/apmd.service ${D}/lib/systemd/system

}

pkg_postinst_append() {

  # don't run flash utils on image install
  if [ "x$D" != "x" ]; then
    exit 1	
  fi

  #uncomment the following line to enable systemd service 
  # ln -s '/lib/systemd/system/apmd.service' '/etc/systemd/system/multi-user.target.wants/apmd.service'

}

