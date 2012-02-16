# do not generate rc-links
PR_append = "+r2"
INITSCRIPT_NAME = "-f networking"
INITSCRIPT_PARAMS = "remove"

FILES_${PN} = " /etc/hosts \
		/etc/protocols \
		/etc/services \
		/etc/rpc \
		/etc/network \
		/usr/sbin  \
"
