# do not generate rc-links
PR_append = "+r2"
INITSCRIPT_NAME = "-f dropbear"
INITSCRIPT_PARAMS = "remove"

FILES_${PN} = "	/var \
		/etc/pam.d \
		/etc/dropbear \
		/etc/default \
		/usr/bin \
		/usr/sbin \
"

