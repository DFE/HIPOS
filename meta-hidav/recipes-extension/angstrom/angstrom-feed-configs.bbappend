PR_append = "+r1"

URI = "http://hydraip-integration:8080/userContent/hidav-"
STABLE = "stable"
UNSTABLE = "unstable"

do_compile() {
	mkdir -p ${S}/${sysconfdir}/opkg

	for feed in all ${MACHINE} ${FEED_ARCH} ; do
		echo "src/gz ${feed} ${URI}${STABLE}/${feed}" > ${S}/${sysconfdir}/opkg/${feed}-${STABLE}-feed.conf
		echo "src/gz ${feed} ${URI}${UNSTABLE}/${feed}" > ${S}/${sysconfdir}/opkg/${feed}-${UNSTABLE}-feed.conf
	done
}

do_install () {
	install -d ${D}${sysconfdir}/opkg
	install -m 0644  ${S}/${sysconfdir}/opkg/* ${D}${sysconfdir}/opkg/
}

RCONFLICTS_${PN} = "${PN}-${UNSTABLE}"
RCONFLICTS_${PN}-${UNSTABLE} = "${PN}"

PACKAGES += "${PN}-${UNSTABLE}"

FILES_${PN} = "${sysconfdir}/opkg/all-${STABLE}-feed.conf \
			   ${sysconfdir}/opkg/${FEED_ARCH}-${STABLE}-feed.conf \
			   ${sysconfdir}/opkg/${MACHINE}-${STABLE}-feed.conf \
			   "

CONFFILES_${PN} = "${sysconfdir}/opkg/all-${STABLE}-feed.conf \
			       ${sysconfdir}/opkg/${FEED_ARCH}-${STABLE}-feed.conf \
			       ${sysconfdir}/opkg/${MACHINE}-${STABLE}-feed.conf \
			       "

FILES_${PN}-${UNSTABLE} = "${sysconfdir}/opkg/all-${UNSTABLE}-feed.conf \
			  			   ${sysconfdir}/opkg/${FEED_ARCH}-${UNSTABLE}-feed.conf \
						   ${sysconfdir}/opkg/${MACHINE}-${UNSTABLE}-feed.conf \
						   "

CONFFILES_${PN}-${UNSTABLE} = "${sysconfdir}/opkg/all-${UNSTABLE}-feed.conf \
			    		      ${sysconfdir}/opkg/${FEED_ARCH}-${UNSTABLE}-feed.conf \
			       			  ${sysconfdir}/opkg/${MACHINE}-${UNSTABLE}-feed.conf \
 					          "

python populate_packages_prepend () {
	etcdir = bb.data.expand('${sysconfdir}/opkg', d)
}
