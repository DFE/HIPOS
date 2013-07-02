DESCRIPTION = "This systemd-service is used to implement the watchdog"
SECTION = "base"
LICENSE = "GPLv2"
PACKAGE_ARCH = "all"
LIC_FILES_CHKSUM = " file://../COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "

# do not generate rc-links
inherit systemd

RDEPENDS_${PN} = " libdrbcc "

PR = "r7"

FILESEXTRAPATHS := "${THISDIR}/files:"

SRC_URI = " file://watchdog.service \
	    file://COPYING "

# systemd
PACKAGES = " ${PN} "

FILES_${PN} = "${base_libdir}/systemd \
               ${sysconfdir}/systemd "

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE = "watchdog.service"

do_install () {
  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/watchdog.service ${D}${base_libdir}/systemd/system/
  install -d ${D}${base_libdir}/systemd/system/multi-user.target.wants
  cd '${D}${base_libdir}/systemd/system/multi-user.target.wants'
  ln -s '../watchdog.service' 'watchdog.service'
  cd -
}
