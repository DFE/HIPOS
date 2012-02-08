# do not generate rc-links
PR_append = "+r1"

pkg_postinst_${PN}_append() {

  rm -f $D/etc/init.d/ppp

}

