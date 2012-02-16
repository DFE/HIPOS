# do not generate rc-links
PR_append = "+r2"
INITSCRIPT_NAME = "-f dbus-1"
INITSCRIPT_PARAMS = "remove"

FILES_${PN} = "	/etc/dbus-1 \
		/etc/default \
		/var/run/dbus \
		/var/lib/dbus \
		/usr/lib/dbus-1.0/test \
		/usr/libexec/dbus-daemon-launch-helper \
		/usr/bin \
		/usr/share/dbus-1 \
"


