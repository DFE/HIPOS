# do not generate rc-links
PRINC := "${@int(PRINC) + 1}"

pkg_postinst_${PN}_append() {

  rm -f $D/etc/init.d/ppp

}

