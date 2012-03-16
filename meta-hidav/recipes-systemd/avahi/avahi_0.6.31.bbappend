# do not generate rc-links
PR_append = "+r1"

pkg_postinst_avahi-daemon_prepend () {
     exit 0
}

pkg_postrm_${PN}-systemd_prepend() {
	exit 0
}

