DESCRIPTION = "Kernel driver for communication with serial attached HIPOS Board Controller"

LICENSE = "GPLv2"
PACKAGE_ARCH = "all"
LIC_FILES_CHKSUM = "file://COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88"

PV = "00.11.12"
PR = "r001"

SRC_URI = "git://github.com/DFE/darmok.git"
SRCREV = "4f15c0427308d224cef134c9ac5c445ed4c99e25" 

S = "${WORKDIR}/git"

