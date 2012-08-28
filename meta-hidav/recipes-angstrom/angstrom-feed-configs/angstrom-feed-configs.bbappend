PR_append = "+r6"


do_compile() {
    mkdir -p ${S}/${sysconfdir}/opkg

    pkg_dir=`echo "${MACHINE}" | sed -e 's/-/-packages-/'`
    URI="http://hydraip-integration:8080/userContent/$pkg_dir"

    for feed in all ${MACHINE} ${FEED_ARCH} ; do
        # replace "-" in machine name with "_" since bitbake/OE will name the 
        # package archive directories alike
        escaped_feed=`echo "${feed}" | sed -e 's/-/_/g'`
        echo "src/gz ${feed} ${URI}/${escaped_feed}" > ${S}/${sysconfdir}/opkg/${feed}-feed.conf
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
