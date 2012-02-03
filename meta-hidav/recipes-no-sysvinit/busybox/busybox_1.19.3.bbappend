# do not generate rc-links
PRINC := "${@int(PRINC) + 3}"
INITSCRIPT_NAME = "-f busybox"
INITSCRIPT_PARAMS = "remove"
INITSCRIPT_NAME_${PN}-httpd = "-f busybox-httpd"
INITSCRIPT_NAME_${PN}-syslog = "-f syslog"
INITSCRIPT_NAME_${PN}-mdev = "-f mdev"
INITSCRIPT_PARAMS_${PN}-mdev = "remove"
INITSCRIPT_NAME_${PN}-udhcpd = "-f busybox-udhcpd" 
INITSCRIPT_NAME_${PN}-udhcpc = "-f busybox-udhcpc" 

pkg_postinst_append() {

  rm -f $D/etc/init.d/syslog
  rm -f $D/etc/init.d/busybox-udhcpc
  rm -f $D/etc/init.d/syslog.busybox
  rm -f $D/etc/init.d/hwclock.sh

}
