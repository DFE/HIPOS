# do not generate rc-links
PR_append = "+r1"
INITSCRIPT_NAME = "-f networking"
INITSCRIPT_PARAMS = "remove"

pkg_postinst_append() {

  rm -f $D/etc/init.d/networking

}
