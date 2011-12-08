PR = "r13"

COMPATIBLE_MACHINE = "hidav"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://hidav-uboot-default-settings.patch \
                   file://hidav-fast-flash-settings.patch \
                   file://hidav-enable-icache.patch \
                   file://hidav-nand-do-not-use-bad_block_table.patch "

BRANCH = "ti81xx-master"
SRCREV = "${AUTOREV}"

SRC_URI = "git://arago-project.org/git/projects/u-boot-omap3.git;branch=${BRANCH};protocol=git"

do_configure() {
    # tfm: do nothing; we re-configure and re-compile u-boot at least ttwwiiccee in the "compile" stage.
    true
}

do_compile() {
    unset LDFLAGS
    unset CFLAGS
    unset CPPFLAGS


    rm -f MLO MLO.nand u-boot-2nd.sd

    bbnote "Building first stage SD card loader (MLO)"
    oe_runmake distclean
    oe_runmake ti8168_evm_min_sd
    oe_runmake -j 8 u-boot.ti
    mv u-boot.min.sd MLO

    bbnote "Building first stage NAND loader (MLO.nand)"
    oe_runmake distclean
    oe_runmake ti8168_evm_config_nand
    oe_runmake -j 8 u-boot.ti
    mv u-boot.noxip.bin MLO.nand

    bbnote "Building second stage SD card loader (u-boot-2nd.sd)"
    oe_runmake distclean
    oe_runmake ti8168_evm_config
    oe_runmake -j 8 all tools env
    mv u-boot.bin u-boot-2nd.sd
}

do_install() { 
    install -d ${D}/boot
    install ${S}/MLO* ${S}/u-boot-2nd.sd ${D}/boot/

    # TODO: add boot loader flash write when firmware flash layout has been defined
}


do_deploy () {
    install -d ${DEPLOY_DIR_IMAGE}
    install ${S}/MLO* ${S}/u-boot-2nd.sd ${DEPLOY_DIR_IMAGE}/
}

