# do not generate rc-links
PRINC := "${@int(PRINC) + 3}"
INITSCRIPT_NAME = "-f dbus-1"
INITSCRIPT_PARAMS = "remove"

pkg_postinst_append() {

  rm -f $D/etc/init.d/dbus-1

}

