DESCRIPTION = "Script tools to use and maintain root fs snapshots by using AUFS writeable overlays."
SECTION = "base"
LICENSE = "GPLv2"
PACKAGE_ARCH = all
LIC_FILES_CHKSUM = " file://COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "
PR = "r13"

# ubi tooling from mtd-utils
RDEPENDS = " mtd-utils "
# no -dbg, -dev, -locale
PACKAGES = " ${PN} " 

COMPATIBLE_MACHINE = "hidav"

SRC_URI=" file://rootfs-overlay-sources/* \
	  file://mount-rootfs-overlay.service   \
	  file://umount-rootfs-overlay.service   \
"

FILES_${PN} += "${base_libdir}/systemd \
	       /overlays \
               /overlays/overlays-data \
               /overlays/overlay-rootfs \
               /overlays/original-rootfs "

PV = "0.1"
S = "${WORKDIR}/rootfs-overlay-sources"

CONFFILES_${PN} = " ${sysconfdir}/default/rootfs-overlay "


do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${WORKDIR}/rootfs-overlay-sources/mount-rootfs-overlay.sh ${D}${sbindir}/
    install -m 0755 ${WORKDIR}/rootfs-overlay-sources/umount-rootfs-overlay.sh ${D}${sbindir}/

    install -d ${D}${sysconfdir}/default
    install -m 0644 ${WORKDIR}/rootfs-overlay-sources/rootfs-overlay-defaults ${D}${sysconfdir}/default/rootfs-overlay

    #
    # You need to update rootfs-overlay-defaults if you change these directories
    #
    install -d ${D}/overlays/overlays-data ${D}/overlays/overlay-rootfs ${D}/overlays/original-rootfs

    install -d ${D}${base_libdir}/systemd/system
    install -m 644 ${WORKDIR}/mount-rootfs-overlay.service ${D}${base_libdir}/systemd/system
    install -m 644 ${WORKDIR}/umount-rootfs-overlay.service ${D}${base_libdir}/systemd/system
    install -d ${D}${base_libdir}/systemd/system/local-fs.target.wants
    install -d ${D}${base_libdir}/systemd/system/shutdown.target.wants
    cd '${D}${base_libdir}/systemd/system/local-fs.target.wants'
    ln -s '../mount-rootfs-overlay.service' 'mount-rootfs-overlay.service'
    cd -
    cd '${D}${base_libdir}/systemd/system/shutdown.target.wants'
    ln -s '../umount-rootfs-overlay.service' 'umount-rootfs-overlay.service'
}
