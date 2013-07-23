DESCRIPTION = "make bootable usb stick for HIPOS"

COMPATIBLE_MACHINE = "hipos-kirkwood"

LICENSE = "MIT"

PR = "r3"

PACKAGES = " ${PN} "

SRC_URI = "file://hipos-bootable-usbstick.sh"

S = "${WORKDIR}"

do_install() {
	install -d ${DEPLOY_DIR_IMAGE}
	install -m 0555 ${WORKDIR}/hipos-bootable-usbstick.sh ${DEPLOY_DIR_IMAGE}/hipos-bootable-usbstick.sh
}

do_configure[noexec] = "1"
do_build[noexec] = "1"
do_compile[noexec] = "1"
do_package[noexec] = "1"
do_packagedata[noexec] = "1"
do_package_write_ipk[noexec] = "1"
do_package_write_deb[noexec] = "1"
do_package_write_rpm[noexec] = "1"

