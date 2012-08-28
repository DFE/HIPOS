DESCRIPTION = "iSCSI Enterprise Target is for building an iSCSI storage system on Linux"
HOMEPAGE = "http://iscsitarget.sourceforge.net/"
LICENSE = "GPL"
PR = "r14"

DEFAULT_PREFERENCE = "-1"

SRC_URI = "svn://iscsitarget.svn.sourceforge.net/svnroot/iscsitarget;module=trunk;rev=HEAD;protocol=http \
           file://init \
           file://ietd.conf \
            "

DEPENDS = " openssl"
RRECOMMENDS_${PN} = "kernel-module-crc32c kernel-module-libcrc32c"

LIC_FILES_CHKSUM = "file://COPYING;md5=6e233eda45c807aa29aeaa6d94bc48a2"

S = "${WORKDIR}/trunk"

inherit module

CFLAGS  = "-isystem${STAGING_KERNEL_DIR}/include -I${STAGING_INCDIR} -L${STAGING_LIBDIR}"
LDFLAGS = "-L${STAGING_LIBDIR}"
FILES_${PN} += " ${base_sbindir}"

do_compile() {
	oe_runmake KSRC=${STAGING_KERNEL_DIR} CFLAGS='${CFLAGS}' LDFLAGS='${LDFLAGS}'
}

do_install() {
	# Module
	install -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/iscsi
	install -m 0644 kernel/iscsi_trgt.ko ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/iscsi/iscsi_trgt.ko

	# Userspace utilities
	install -d ${D}${base_sbindir}
        install -m 0755 usr/ietd ${D}${base_sbindir}/ietd
        install -m 0755 usr/ietadm ${D}${base_sbindir}/ietadm

	# Config files, init scripts
	mkdir -p ${D}${sysconfdir}/init.d
	install -m 0755 ../init ${D}${sysconfdir}/init.d/iscsi-target
	install -m 0644 ${WORKDIR}/ietd.conf ${D}${sysconfdir}/
	install -m 0644 etc/initiators.allow ${D}${sysconfdir}/
	install -m 0644 etc/initiators.deny ${D}${sysconfdir}/
}
