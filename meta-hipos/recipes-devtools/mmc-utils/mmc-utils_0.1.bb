DESCRIPTION = "Userspace tools for MMC/SD devices"
SECTION = "console/utils"
LICENSE = "GPLv2"

LIC_FILES_CHKSUM = "file://mmc.h;beginline=2;endline=15;md5=ee6d6c65ecf0c8a7c1539630cd0d1ea2"

SRCREV = "21bb473fc58366b872efe31e1da7831cad4b92fa"

PR = "r0"

SRC_URI = "git://git.kernel.org/pub/scm/linux/kernel/git/cjb/mmc-utils.git;protocol=git"

S = "${WORKDIR}/git"

do_install() {
	install -d ${D}${sbindir}/
	install -m 0755 ${S}/mmc ${D}${sbindir}/
}

