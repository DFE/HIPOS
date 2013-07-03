DESCRIPTION = "This systemd-service is used to get serial number"
SECTION = "base"
LICENSE = "GPLv2"
PACKAGE_ARCH = "all"
LIC_FILES_CHKSUM = " file://../COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "


# do not generate rc-links
inherit systemd
RDEPENDS_${PN} = " hipos-network-setup "

PR = "r6"

FILESEXTRAPATHS := "${THISDIR}/files:"

SRC_URI = " file://hipos-serial-number.service  \
            file://hipos-serial-number.sh \
	    file://COPYING "

# systemd
PACKAGES = " ${PN} "

FILES_${PN} = "${base_libdir}/systemd \
               ${sysconfdir}/hipos/scripts/hipos-serial-number.sh \
		"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE = "hipos-serial-number.service"

do_install () {
  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/hipos-serial-number.service ${D}${base_libdir}/systemd/system/
  install -d ${D}${sysconfdir}/hipos/scripts
  install -m 0755 ${WORKDIR}/hipos-serial-number.sh ${D}${sysconfdir}/hipos/scripts/
  install -d ${D}${base_libdir}/systemd/system/multi-user.target.wants
  cd '${D}${base_libdir}/systemd/system/multi-user.target.wants'
  ln -s '../hipos-serial-number.service' 'hipos-serial-number.service'
  cd -
}


