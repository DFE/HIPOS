DESCRIPTION = "HidaV make bootable sd card with dev image"

LICENSE = "MIT"

PR = "r9"

SRC_URI = "file://ti814x-bootable-sdcard.sh \
           file://ti816x-bootable-sdcard.sh"

do_install() {
	install -d ${DEPLOY_DIR_IMAGE}
	install -m 0555 ${WORKDIR}/ti814x-bootable-sdcard.sh ${DEPLOY_DIR_IMAGE}/ti814x-bootable-sdcard_${PR}.sh
	install -m 0555 ${WORKDIR}/ti816x-bootable-sdcard.sh ${DEPLOY_DIR_IMAGE}/ti816x-bootable-sdcard_${PR}.sh
}

do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_build[noexec] = "1"
do_compile[noexec] = "1"
do_package[noexec] = "1"
do_package_write_ipk[noexec] = "1"
do_package_write_deb[noexec] = "1"
do_package_write_rpm[noexec] = "1"
