PR_append = "+r3"

COMPATIBLE_MACHINE = "hidav"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://hidav-uboot-default-settings.patch \
                   file://hidav-fast-flash-settings.patch \
                   file://hidav-enable-icache.patch \
                   file://hidav-nand-do-not-use-bad_block_table.patch "

BRANCH_ti814x = "ti81xx-master"
SRCREV_pn-${PN}_ti814x  = "2ec1a17817e422b9417289b91c027980b45c7d65"

do_configure() {
    # tfm: do nothing; we re-configure and re-compile u-boot at least ttwwiiccee in the "compile" stage.
    true
}

do_compile() {
    unset LDFLAGS
    unset CFLAGS
    unset CPPFLAGS

    rm   -rf _hidav_814x _hidav_816x
    mkdir -p _hidav_814x _hidav_816x

    rm -f MLO_ti816x MLO.nand_ti816x u-boot-2nd.sd_ti816x

    bbnote "Building first stage SD card loader (MLO) ti816x"
    oe_runmake distclean
    oe_runmake ti8168_evm_min_sd
    oe_runmake -j 8 u-boot.ti TI_IMAGE=MLO
    mv MLO _hidav_816x/

    bbnote "Building first stage NAND loader (MLO.nand) ti816x"
    oe_runmake distclean
    oe_runmake ti8168_evm_config_nand
    oe_runmake -j 8 u-boot.ti TI_IMAGE=MLO.nand
    mv MLO.nand _hidav_816x/

    bbnote "Building second stage SD card loader (u-boot-2nd.sd) ti816x"
    oe_runmake distclean
    oe_runmake ti8168_evm_config
    oe_runmake -j 8 u-boot.ti TI_IMAGE=u-boot-2nd.sd
    mv u-boot-2nd.sd _hidav_816x/



    bbnote "Building first stage SD card loader (MLO) ti814x"
    oe_runmake distclean
    oe_runmake ti8148_evm_min_sd
    oe_runmake -j 8 u-boot.ti TI_IMAGE=MLO
    mv MLO _hidav_814x/

    bbnote "Building first stage NAND loader (MLO.nand) ti814x"
    oe_runmake distclean
    oe_runmake ti8148_evm_min_nand
    oe_runmake -j 8 u-boot.ti TI_IMAGE=MLO.nand
    mv MLO.nand _hidav_814x/

    bbnote "Building second stage SD card loader (u-boot-2nd.sd) ti814x"
    oe_runmake distclean
    oe_runmake ti8148_evm_config_sd
    oe_runmake -j 8 u-boot.ti TI_IMAGE=u-boot-2nd.sd
    mv u-boot-2nd.sd _hidav_814x/
}

do_install() { 
    true
}


do_deploy () {
    install -d ${DEPLOY_DIR_IMAGE}
    install -d ${DEPLOY_DIR_IMAGE}/_boot_ti816x/
    install -d ${DEPLOY_DIR_IMAGE}/_boot_ti814x/

    install ${S}/_hidav_816x/* ${DEPLOY_DIR_IMAGE}/_boot_ti816x/
    install ${S}/_hidav_814x/* ${DEPLOY_DIR_IMAGE}/_boot_ti814x/
}

