# do not generate rc-links
PR_append = "+r3"

FILES_${PN} = "	/var \
		/etc/pam.d \
		/etc/dropbear \
		/etc/default \
		/usr/bin \
		/usr/sbin \
"

pkg_postrm_${PN}-systemd_prepend() {
	exit 0
}

