DESCRIPTION = "make bootable usb stick for HIPOS"

COMPATIBLE_MACHINE = "hipos-kirkwood"

LICENSE = "MIT"

PR = "r2"

PACKAGES = " ${PN} "

DEPENDS = "hipos-devimage hipos-prodimage virtual/kernel"

SRC_URI = "file://hipos-bootable-usbstick.sh"
           

do_install() {
	install -d ${DEPLOY_DIR_IMAGE}
	install -m 0555 ${WORKDIR}/hipos-bootable-usbstick.sh ${DEPLOY_DIR_IMAGE}/hipos-bootable-usbstick.sh
}

do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_build[noexec] = "1"
do_compile[noexec] = "1"
do_package[noexec] = "1"
do_packagedata[noexec] = "1"
do_package_write_ipk[noexec] = "1"
do_package_write_deb[noexec] = "1"
do_package_write_rpm[noexec] = "1"

