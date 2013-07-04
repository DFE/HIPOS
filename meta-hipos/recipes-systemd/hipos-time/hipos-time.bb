DESCRIPTION = "This systemd-service is used to set system time using the rtc on the board controller"
SECTION = "base"
LICENSE = "GPLv2"
PACKAGE_ARCH = "all"
LIC_FILES_CHKSUM = " file://../COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "

# do not generate rc-links
inherit systemd

RDEPENDS_${PN} = " libdrbcc \
                   gawk "

PR = "r4"

FILESEXTRAPATHS := "${THISDIR}/files:"

SRC_URI = " file://hipos-time.service \
            file://set-time.sh \
            file://COPYING "

# systemd
PACKAGES = " ${PN} "

FILES_${PN} = "${base_libdir}/systemd \
               ${sysconfdir}/systemd \
               ${sysconfdir}/hipos/scripts/set-time.sh "

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE = "hipos-time.service"

do_install () {
  install -d ${D}${base_libdir}/systemd/system
  install -m 0644 ${WORKDIR}/hipos-time.service ${D}${base_libdir}/systemd/system/
  
  install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants
  cd '${D}${sysconfdir}/systemd/system/multi-user.target.wants'
  ln -s '../../../..${base_libdir}/systemd/system/hipos-time.service' 'hipos-time.service'
  
  install -d ${D}${sysconfdir}/hipos/scripts
  install -m 0755 ${WORKDIR}/set-time.sh ${D}${sysconfdir}/hipos/scripts
  
  cd -
}
