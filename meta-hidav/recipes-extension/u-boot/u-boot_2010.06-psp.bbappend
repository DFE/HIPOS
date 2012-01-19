PR = "r18"

COMPATIBLE_MACHINE = "hidav"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://hidav-uboot-default-settings.patch \
                   file://hidav-fast-flash-settings.patch \
                   file://hidav-enable-icache.patch \
                   file://hidav-nand-do-not-use-bad_block_table.patch "

BRANCH_ti814x = "ti81xx-master"
SRCREV_pn-${PN}_ti814x  = "456a38217871f9184b65efd42e010a264b4d65e1"

do_configure() {
    # tfm: do nothing; we re-configure and re-compile u-boot at least ttwwiiccee in the "compile" stage.
    true
}

do_compile() {
    unset LDFLAGS
    unset CFLAGS
    unset CPPFLAGS


    rm -f MLO_ti816x MLO.nand_ti816x u-boot-2nd.sd_ti816x

    bbnote "Building first stage SD card loader (MLO) ti816x"
    oe_runmake distclean
    oe_runmake ti8168_evm_min_sd
    oe_runmake -j 8 u-boot.ti TI_IMAGE=MLO_ti816x

    bbnote "Building first stage NAND loader (MLO.nand) ti816x"
    oe_runmake distclean
    oe_runmake ti8168_evm_config_nand
    oe_runmake -j 8 u-boot.ti TI_IMAGE=MLO.nand_ti816x

    bbnote "Building second stage SD card loader (u-boot-2nd.sd) ti816x"
    oe_runmake distclean
    oe_runmake ti8168_evm_config
    oe_runmake -j 8 u-boot.ti TI_IMAGE=u-boot-2nd.sd_ti816x


    rm -f MLO MLO.nand u-boot-2nd.sd

    bbnote "Building first stage SD card loader (MLO) ti814x"
    oe_runmake distclean
    oe_runmake ti8148_evm_min_sd
    oe_runmake -j 8 u-boot.ti TI_IMAGE=MLO

    bbnote "Building first stage NAND loader (MLO.nand) ti814x"
    oe_runmake distclean
    oe_runmake ti8148_evm_config_nand
    oe_runmake -j 8 u-boot.ti TI_IMAGE=MLO.nand

    bbnote "Building second stage SD card loader (u-boot-2nd.sd) ti814x"
    oe_runmake distclean
    oe_runmake ti8148_evm_config
    oe_runmake -j 8 u-boot.ti TI_IMAGE=u-boot-2nd.sd
}

do_install() { 
    install -d ${D}/boot
    install ${S}/MLO ${D}/boot/
    install ${S}/MLO.nand ${D}/boot/
    install ${S}/u-boot-2nd.sd ${D}/boot/

    # TODO: add boot loader flash write when firmware flash layout has been defined
}


do_deploy () {
    install -d ${DEPLOY_DIR_IMAGE}
    install -d ${DEPLOY_DIR_IMAGE}/ti816x
    install ${S}/MLO_ti816x ${DEPLOY_DIR_IMAGE}/ti816x/MLO
    install ${S}/MLO.nand_ti816x ${DEPLOY_DIR_IMAGE}/ti816x/MLO.nand
    install ${S}/u-boot-2nd.sd_ti816x ${DEPLOY_DIR_IMAGE}/ti816x/u-boot-2nd.sd
    install ${S}/MLO ${DEPLOY_DIR_IMAGE}/
    install ${S}/MLO.nand ${DEPLOY_DIR_IMAGE}/
    install ${S}/u-boot-2nd.sd ${DEPLOY_DIR_IMAGE}/
}

