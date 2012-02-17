# do not generate rc-links
PR_append = "+r3"

FILES_${PN} = "	/etc/dbus-1 \
		/etc/default \
		/var/run/dbus \
		/var/lib/dbus \
		/usr/lib/dbus-1.0/test \
		/usr/libexec/dbus-daemon-launch-helper \
		/usr/bin \
		/usr/share/dbus-1 \
"

pkg_postrm_${PN}-systemd_prepend() {
	exit 0
}


