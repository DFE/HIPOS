PR_append = "+r2"

FILESEXTRAPATHS_prepend := "${THISDIR}/udev-182:"

SRC_URI_append = " file://udev-dont-init-blockrom.patch "

# tfm:_temporary workaround for libkmod (package kmod, openembedded-core) putting its .pc file to /lib/pkgconfig
#  instead of /usr/lib/pkgconfig or /usr/share/pkgconfig.
#  See openembedded mailing list for progress on this issue: <http://thread.gmane.org/gmane.comp.handhelds.openembedded/52131>

PKG_CONFIG_PATH = "${PKG_CONFIG_DIR}:${STAGING_DATADIR}/pkgconfig:${PKG_CONFIG_SYSROOT_DIR}/${base_libdir}/pkgconfig/"

