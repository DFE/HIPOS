DESCRIPTION = "This systemd-service is used to setup network phys"
SECTION = "base"
LICENSE = "GPLv2"
PACKAGE_ARCH = "all"
LIC_FILES_CHKSUM = " file://../COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "


# do not generate rc-links
inherit systemd

RDEPENDS_${PN} = " ethtool "

PR = "r1"

FILESEXTRAPATHS := "${THISDIR}/files:"

SRC_URI = " file://network-setup.service  \
            file://network-setup.sh \
	    file://COPYING "

# systemd
PACKAGES = " ${PN} "

FILES_${PN} = "${base_libdir}/systemd \
               ${sysconfdir}/network-setup.sh \
		"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE = "network-setup.service"

do_install () {
  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/network-setup.service ${D}${base_libdir}/systemd/system/
  install -d ${D}/etc
  install -m 0755 ${WORKDIR}/network-setup.sh ${D}${sysconfdir}
  install -d ${D}${base_libdir}/systemd/system/multi-user.target.wants
  cd '${D}${base_libdir}/systemd/system/multi-user.target.wants'
  ln -s '../network-setup.service' 'network-setup.service'
  cd -
}


