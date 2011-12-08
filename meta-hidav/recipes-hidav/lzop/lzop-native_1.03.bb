#
# Needed by Kernel (LZO Compression) but not provided by OE. Sources doen't configure properly so provide the binary.
#

DESCRIPTION = "LZO compressor"

LICENSE = "MIT"

PR = "r1"

SRC_URI = "file://lzop \
           "

do_install() {
	install -m 0555 ${WORKDIR}/lzop ${STAGING_BINDIR_NATIVE}
}

do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_build[noexec] = "1"
do_compile[noexec] = "1"
do_package[noexec] = "1"
do_package_write_ipk[noexec] = "1"
do_package_write_deb[noexec] = "1"
do_package_write_rpm[noexec] = "1"