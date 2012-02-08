# do not generate rc-links
PR_append = "+r1"
INITSCRIPT_NAME = "-f dropbear"
INITSCRIPT_PARAMS = "remove"

pkg_postinst_append() {

  rm -f $D/etc/init.d/dropbear

}

