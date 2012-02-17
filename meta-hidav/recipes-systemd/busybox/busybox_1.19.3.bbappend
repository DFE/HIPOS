# do not generate rc-links
PR_append = "+r3"

FILES_${PN}= "	/etc/busybox.links \
		/etc/default \
		/etc/default/busybox-syslog \
		/bin/busybox \
		/bin/sh \
"

FILES_${PN}-httpd = ""
FILES_${PN}-syslog = " 	/etc/syslog-startup.conf.busybox "
FILES_${PN}-mdev  = " 	/etc/mdev.conf "
FILES_${PN}-udhcpd = ""
FILES_${PN}-udhcpc = " 	/etc/udhcpc.d \
			/usr \
"

pkg_postinst_${PN}-syslog_prepend () {
        update-alternatives --install ${sysconfdir}/syslog-startup.conf syslog-startup-conf syslog-startup.conf.${BPN} 50
	exit 0
}

pkg_prerm_${PN}-syslog_prepend () {
        # remove syslog
        update-alternatives --remove syslog-startup-conf syslog-startup.conf.${BPN}
	exit 0
}

pkg_postrm_${PN}-httpd_prepend() {
	exit 0
}
pkg_postrm_${PN}-syslog_prepend() {
        exit 0
}
pkg_postrm_${PN}-mdev_prepend() {
        exit 0
}
pkg_postrm_${PN}-udhcpd_prepend() {
        exit 0
}
pkg_postrm_${PN}-udhcpc_prepend() {
        exit 0
}


