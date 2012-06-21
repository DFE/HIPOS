PR_append = "+r3"

URI = "http://hydraip-integration:8080/userContent/hidav-packages"

do_compile() {
	mkdir -p ${S}/${sysconfdir}/opkg

	for feed in all ${MACHINE} ${FEED_ARCH} ; do
		echo "src/gz ${feed} ${URI}/${feed}" > ${S}/${sysconfdir}/opkg/${feed}-feed.conf
	done
}

do_install () {
	install -d ${D}${sysconfdir}/opkg
	install -m 0644  ${S}/${sysconfdir}/opkg/* ${D}${sysconfdir}/opkg/
}


FILES_${PN} = "${sysconfdir}/opkg/all-feed.conf \
			   ${sysconfdir}/opkg/${FEED_ARCH}-feed.conf \
			   ${sysconfdir}/opkg/${MACHINE}-feed.conf \
			   "

CONFFILES_${PN} = "${sysconfdir}/opkg/all-feed.conf \
			       ${sysconfdir}/opkg/${FEED_ARCH}-feed.conf \
			       ${sysconfdir}/opkg/${MACHINE}-feed.conf \
			       "


python populate_packages_prepend () {
	etcdir = bb.data.expand('${sysconfdir}/opkg', d)
}
