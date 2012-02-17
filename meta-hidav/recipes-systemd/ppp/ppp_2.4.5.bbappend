# do not generate rc-links
PR_append = "+r3"

FILES_${PN} = "	${bindir} \
		${sysconfdir}/ppp/ip-up \
		${sysconfdir}/ppp \
		/usr \
"

pkg_postrm_${PN}-systemd_prepend() {
	exit 0
}

