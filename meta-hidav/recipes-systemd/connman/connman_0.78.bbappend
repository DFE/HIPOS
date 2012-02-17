# do not generate rc-links
PR_append = "+r3"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += "file://connman.service"

do_install_append() {
    install -d ${D}${base_libdir}/systemd/system
    install -m 644 ${WORKDIR}/connman.service ${D}${base_libdir}/systemd/system
}


FILES_${PN} = "	${bindir}/* \
		${sbindir}/* \
		${libexecdir}/* \
		${libdir}/lib*.so.* \
            	${sharedstatedir} \
		${localstatedir} \
            	${base_bindir}/* \
		${base_sbindir}/* \
		${base_libdir}/*.so* \
		${datadir}/${PN} \
            	${datadir}/pixmaps \
		${datadir}/applications \
            	${datadir}/idl \
		${datadir}/omf \
		${datadir}/sounds \
            	${libdir}/bonobo/servers \
            	${datadir}/dbus-1/system-services/* \
		${sysconfdir}/dbus-1 \
		${sysconfdir}/dbus-1/system.d \
		${sysconfdir}/dbus-1/system.d/connman.conf \
"

pkg_postrm_${PN}-systemd_prepend() {
	exit 0
}

