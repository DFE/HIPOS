# do not generate rc-links
PR_append = "+r2"
INITSCRIPT_NAME = "-f busybox"
INITSCRIPT_PARAMS = "remove"
INITSCRIPT_NAME_${PN}-httpd = "-f busybox-httpd"
INITSCRIPT_NAME_${PN}-syslog = "-f syslog"
INITSCRIPT_NAME_${PN}-mdev = "-f mdev"
INITSCRIPT_PARAMS_${PN}-mdev = "remove"
INITSCRIPT_NAME_${PN}-udhcpd = "-f busybox-udhcpd" 
INITSCRIPT_NAME_${PN}-udhcpc = "-f busybox-udhcpc" 

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

