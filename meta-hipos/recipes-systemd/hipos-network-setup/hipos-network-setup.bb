DESCRIPTION = "This systemd-service is used to setup network phys"
SECTION = "base"
LICENSE = "GPLv2"
PACKAGE_ARCH = "all"
LIC_FILES_CHKSUM = " file://../COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "

# do not generate rc-links
inherit systemd

RDEPENDS_${PN} = " ethtool "

PR = "r8"

FILESEXTRAPATHS := "${THISDIR}/files:"

SRC_URI = " file://hipos-network-setup.service  \
            file://hipos-network-setup.sh \
	    file://COPYING "

# systemd
PACKAGES = " ${PN} "

FILES_${PN} = "${base_libdir}/systemd \
               ${sysconfdir}/hipos/hipos-network-setup.sh \
		"

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE = "hipos-network-setup.service"

do_install () {
  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/hipos-network-setup.service ${D}${base_libdir}/systemd/system/
  install -d ${D}${sysconfdir}/hipos
  install -m 0755 ${WORKDIR}/hipos-network-setup.sh ${D}${sysconfdir}/hipos/
  install -d ${D}${base_libdir}/systemd/system/multi-user.target.wants
  cd '${D}${base_libdir}/systemd/system/multi-user.target.wants'
  ln -s '../hipos-network-setup.service' 'hipos-network-setup.service'
  cd -
}

