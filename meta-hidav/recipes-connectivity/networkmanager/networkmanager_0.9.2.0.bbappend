inherit systemd

PACKAGES =+ " ${PN}-systemd "

FILES_${PN} = " \
		${libexecdir} \
		${libdir}/pppd/*/nm-pppd-plugin.so \
		${libdir}/NetworkManager/*.so \
		${datadir}/polkit-1 \
		${datadir}/dbus-1 \
		${base_libdir}/udev/* \
		/etc/dbus-1 \
		/etc/NetworkManager \
		/var/run/NetworkManager \
		/var/lib \
		/usr/lib/libnm-glib-vpn.so.1.1.0 \
		/usr/lib/libnm-glib.so.4.2.0 \
		/usr/lib/libnm-glib.so.4 \
		/usr/lib/libnm-glib-vpn.so.1 \
		/usr/sbin \
		/usr/bin \
"

SYSTEMD_PACKAGES = "${PN}-systemd"
SYSTEMD_SERVICE = "NetworkManager.service"

FILES_${PN}-systemd += "${base_libdir}/systemd"
RDEPENDS_${PN}-systemd += "${PN} systemd"

PR_append = "+r5"

# use no iptables
EXTRA_OECONF = " \
		--with-distro=debian \
		--with-crypto=gnutls \
		--disable-more-warnings \
        --with-dhclient=${base_sbindir}/dhclient \
"

RRECOMMENDS_${PN} = " "

pkg_postinst_${PN}() {
    exit 0
}

pkg_postinst_${PN}-systemd() {
    OPTS=""

    if [ -n "$D" ]; then
        OPTS="--root=$D"
    fi

    systemctl $OPTS enable ${SYSTEMD_SERVICE}

    if [ -z "$D" ]; then
        systemctl start ${SYSTEMD_SERVICE}
    fi

}


pkg_postrm_${PN}() {
    exit 0
}

pkg_postrm_${PN}-systemd() {
    exit 0
}

pkg_prerm_${PN}() {
    exit 0
}

pkg_prerm_${PN}-systemd() {
    if [ -z "$D" ]; then
	systemctl disable ${SYSTEMD_SERVICE}
        systemctl stop ${SYSTEMD_SERVICE}
    fi
}

