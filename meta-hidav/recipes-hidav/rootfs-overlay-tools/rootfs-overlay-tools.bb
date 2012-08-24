DESCRIPTION = "Script tools to use and maintain root fs snapshots by using AUFS writeable overlays."
SECTION = "base"
LICENSE = "GPLv2"
PACKAGE_ARCH = "all"
LIC_FILES_CHKSUM = " file://COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "

PR = "r1"

# ubi tooling from mtd-utils
RDEPENDS = " mtd-utils "
# no -dbg, -dev, -locale
PACKAGES = " ${PN} "

SRC_URI=" file://rootfs-overlay-tools-sources/* \
"


PV = "0.1"
S = "${WORKDIR}/rootfs-overlay-tools-sources"

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 ${WORKDIR}/rootfs-overlay-tools-sources/overlay-* ${D}${sbindir}/

}

do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_build[noexec] = "1"

do_compile() {

	./test-overlay
}

