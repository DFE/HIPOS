# do not generate rc-links
PR_append = "+r1"
RDEPENDS_${PN} += "systemd"

pkg_postinst_wpa-supplicant () {
        # If we're offline, we don't need to do this.
        if [ "x$D" != "x" ]; then
                exit 0
        fi

        DBUSPID=`pidof dbus-daemon`

        if [ "x$DBUSPID" != "x" ]; then
               systemctl reload-or-try-restart dbus.service
        fi
}

