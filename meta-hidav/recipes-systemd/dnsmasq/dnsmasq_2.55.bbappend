# do not generate rc-links
PR_append = "+r2"
INITSCRIPT_NAME = "-f dnsmasq"
INITSCRIPT_PARAMS = "remove"

FILES_${PN} = "	/etc/dnsmasq.conf \
		/etc/dnsmasq.d \
		/usr \
"
