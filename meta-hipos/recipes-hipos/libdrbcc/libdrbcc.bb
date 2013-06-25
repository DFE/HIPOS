DESCRIPTION = "HIPOS Boardcontroller communication tool"
SECTION = "libs"
DEPENDS = "readline"

LICENSE = "GPLv3 LGPLv3"
LIC_FILES_CHKSUM = "file://COPYING;md5=d32239bcb673463ab874e80d47fae504 \
		    file://COPYING.LIB;md5=e6a600fd5e1d9cbde2d983680233ad02 "

PR = "r1"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/DFE/libdrbcc.git"
SRCREV_default_pn-libdrbcc = "cc055c9ed164b67ceccfe90ec2287bc3f6bd3584"

inherit autotools


