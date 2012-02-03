# do not generate rc-links
PRINC := "${@int(PRINC) + 1}"

pkg_postinst_append() {

  rm -f $D/etc/init.d/avahi-daemon

}

