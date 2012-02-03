# do not generate rc-links
PRINC := "${@int(PRINC) + 4}"
INITSCRIPT_NAME = "-f networking"
INITSCRIPT_PARAMS = "remove"

pkg_postinst_append() {

  rm -f $D/etc/init.d/networking

}
