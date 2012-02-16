# do not generate rc-links
PRINC := "${@int(PRINC) + 2}"

SRC_URI = "http://avahi.org/download/avahi-${PV}.tar.gz \
          file://00avahi-autoipd \
          file://99avahi-autoipd \
          file://fix_for_automake_1.11.2.patch"


FILES_avahi-daemon = "${sbindir}/avahi-daemon \
                      ${sysconfdir}/avahi/avahi-daemon.conf \
                      ${sysconfdir}/avahi/hosts \
                      ${sysconfdir}/avahi/services \
                      ${sysconfdir}/dbus-1 \
                      ${datadir}/avahi/introspection/*.introspect \
                      ${datadir}/avahi/avahi-service.dtd \
                      ${datadir}/avahi/service-types \
                      ${datadir}/dbus-1/interfaces \
                      ${datadir}/dbus-1/system-services"
FILES_avahi-dnsconfd = "${sbindir}/avahi-dnsconfd \
                        ${sysconfdir}/avahi/avahi-dnsconfd.action"

FILES_${PN} = "	${sbindir} \
		${sysconfdir}/avahi \
		${sysconfdir}/dbus-1 \
		${datadir} \
		/var \
		${sysconfdir}/dhcp \
"

pkg_postinst_avahi-daemon_prepend () {
     exit 0
}

