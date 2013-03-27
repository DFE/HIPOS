DESCRIPTION = "HidaV boot configuration user space tooling"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = " file://COPYING;md5=9ac2e7cff1ddaf48b6eab6028f23ef88 "

PR = "r10"
PV = "1.1"

export LDFLAGS += "-L${STAGING_LIBDIR}/libmtd.a" 

DEPENDS = "mtd-utils test-harness-native"
SRC_URI="file://${PN}-sources-${PV}"
S = "${WORKDIR}/${PN}-sources-${PV}"

inherit autotools 

# tfm: build + run unit tests on the build host
do_make_check() {
    oe_runmake ${PARALLEL_MAKE} check
    echo " ##########################################"
    echo " ########### UNIT TEST RUN LOGS ###########"
    echo " ##########################################"
    for file in ${S}/src/test-*.log; do 
        echo -e "\n\n\n\n ##############################################################"
        echo "    ########### $(basename $file) "
        cat $file
    done
}

addtask make_check after do_compile before do_install
