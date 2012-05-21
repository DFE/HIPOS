DESCRIPTION = "This systemd-service is used to mark bootconfig healthy"
SECTION = "base"
LICENSE = "GPLv2"
PACKAGE_ARCH = "all"
LIC_FILES_CHKSUM = " file://../COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "


# do not generate rc-links
inherit systemd

RDEPENDS = " bootconfig "

PR = "r3"

FILESEXTRAPATHS := "${THISDIR}/files:"

SRC_URI = " file://healthy.service \
	    file://COPYING "

# systemd
PACKAGES = " ${PN} "

FILES_${PN} = "${base_libdir}/systemd"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE = "healthy.service"

do_install () {
  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/healthy.service ${D}${base_libdir}/systemd/system/
  install -d ${D}${base_libdir}/systemd/system/multi-user.target.wants
  cd '${D}${base_libdir}/systemd/system/multi-user.target.wants'
  ln -s '../healthy.service' 'healthy.service'
  cd -
}


