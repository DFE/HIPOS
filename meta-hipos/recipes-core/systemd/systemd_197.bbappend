# do not generate rc-links
PR_append = "+r4"
DEPENDS += " grep "

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append = " \
	       file://0001-systemd-halt.service-always-poweroff-on-halt.patch \
	       file://0001-systemd-reboot.service-always-poweroff-on-reboot.patch \
"

# tfm:_temporary workaround for libkmod (package kmod, openembedded-core) putting its .pc file to /lib/pkgconfig
#  instead of /usr/lib/pkgconfig or /usr/share/pkgconfig.
#  See openembedded mailing list for progress on this issue: <http://thread.gmane.org/gmane.comp.handhelds.openembedded/52131>

PKG_CONFIG_PATH = "${PKG_CONFIG_DIR}:${STAGING_DATADIR}/pkgconfig:${PKG_CONFIG_SYSROOT_DIR}/${base_libdir}/pkgconfig/"


do_install_append() {
	if ! grep "Restart=" ${D}/lib/systemd/system/systemd-journald.service ; then
		echo "RestartSec= 1" >>${D}/lib/systemd/system/systemd-journald.service
		echo "Restart= always" >>${D}/lib/systemd/system/systemd-journald.service
	fi
}
