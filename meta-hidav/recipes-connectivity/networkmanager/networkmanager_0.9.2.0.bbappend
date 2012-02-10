inherit systemd

PACKAGES =+ " ${PN}-systemd "

FILES_${PN} += " \
		${libexecdir} \
		${libdir}/pppd/*/nm-pppd-plugin.so \
		${libdir}/NetworkManager/*.so \
		${datadir}/polkit-1 \
		${datadir}/dbus-1 \
		${base_libdir}/udev/* \
"

SYSTEMD_PACKAGES = "${PN}-systemd"
SYSTEMD_SERVICE = "NetworkManager.service"

FILES_${PN}-systemd += "${base_libdir}/systemd"
RDEPENDS_${PN}-systemd += "${PN}"

PR_append = "+r2"

# use no iptables
EXTRA_OECONF = " \
		--with-distro=debian \
		--with-crypto=gnutls \
		--disable-more-warnings \
        --with-dhclient=${base_sbindir}/dhclient \
"

RRECOMMENDS_${PN} = " "

pkg_postinst_${PN}() {
	true
}

pkg_prerm_${PN}() {
	true
}

