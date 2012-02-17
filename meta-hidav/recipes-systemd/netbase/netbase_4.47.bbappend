# do not generate rc-links
PR_append = "+r3"

FILES_${PN} = " /etc/hosts \
		/etc/protocols \
		/etc/services \
		/etc/rpc \
		/etc/network \
		/usr/sbin  \
"

pkg_postrm_${PN}-systemd_prepend() {
	exit 0
}

