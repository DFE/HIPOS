DESCRIPTION = "This systemd-service is used to get serial number"
SECTION = "base"
LICENSE = "GPLv2"
PACKAGE_ARCH = "all"
LIC_FILES_CHKSUM = " file://../COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "


# do not generate rc-links
inherit systemd

RDEPENDS_${PN} = " network-setup "

PR = "r4"

FILESEXTRAPATHS := "${THISDIR}/files:"

SRC_URI = " file://serial-number.service  \
            file://serial-number.sh \
	    file://COPYING "

# systemd
PACKAGES = " ${PN} "

FILES_${PN} = "${base_libdir}/systemd \
               ${sysconfdir}/serial-number.sh \
		"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE = "serial-number.service"

do_install () {
  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/serial-number.service ${D}${base_libdir}/systemd/system/
  install -d ${D}${sysconfdir}
  install -m 0755 ${WORKDIR}/serial-number.sh ${D}${sysconfdir}
  install -d ${D}${base_libdir}/systemd/system/multi-user.target.wants
  cd '${D}${base_libdir}/systemd/system/multi-user.target.wants'
  ln -s '../serial-number.service' 'serial-number.service'
  cd -
}


