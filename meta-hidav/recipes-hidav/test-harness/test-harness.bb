DESCRIPTION = "HidaV test harness header file - easy mocking for C unit tests"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = " file://COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "

PR = "r1"
PV = "1.2"

PACKAGES = " ${PN} "
SRC_URI="file://test-harness/"
S = "${WORKDIR}/test-harness/"

FILES_${PN} = " ${includedir}/test_harness/test_harness.h \
		${includedir}/test_harness/test_harness_h_KISS_testing_for_C.pdf \
		"

do_install() {
	install -d ${D}${includedir}/test_harness/
	install -m 0644 ${S}include/* ${D}${includedir}/test_harness/
}

do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_build[noexec] = "1"
do_compile[noexec] = "1"

