# do not generate rc-links
PRINC := "${@int(PRINC) + 3}"
INITSCRIPT_NAME = "-f networking"
INITSCRIPT_PARAMS = "remove"

# TI do not like "$THISDIR". WHY? (The following line is a workaround. "$FUCK_TI" is my new "$THISDIR".)
FUCK_TI := "${@os.path.dirname(bb.data.getVar('FILE', d, True))}"

FILESEXTRAPATHS_prepend := "${FUCK_TI}/files"
SRC_URI_append = " file://network.service "
FILES_${PN}-systemd += " /lib/systemd/system/network.service "

PACKAGES += "${PN}-systemd"

do_install_append () {

  install -d ${D}/lib/systemd/system
  install -m 0644 ${WORKDIR}/network.service ${D}/lib/systemd/system

}

pkg_postinst_append() {

  rm -f $D/etc/init.d/networking
  # don't run flash utils on image install
  if [ "x$D" != "x" ]; then
    exit 1
  fi

  ln -f -s '/lib/systemd/system/network.service' '/etc/systemd/system/network.target.wants/network.service'
  ln -f -s '/lib/systemd/system/network.service' '/etc/systemd/system/multi-user.target.wants/network.service'

}
