# do not generate rc-links
PR_append = "+r3"

FILES_${PN} = "	/etc/dnsmasq.conf \
		/etc/dnsmasq.d \
		/usr \
"

pkg_postrm_${PN}-systemd_prepend() {
	exit 0
}

