UPDATERCPN ?= "${PN}"

DEPENDS_append = " update-rc.d-native"
UPDATERCD = "update-rc.d"
UPDATERCD_virtclass-native = ""
UPDATERCD_virtclass-nativesdk = ""

RDEPENDS_${UPDATERCPN}_append = " ${UPDATERCD}"

INITSCRIPT_PARAMS ?= "defaults"

INIT_D_DIR = "${sysconfdir}/init.d"

updatercd_postinst() {
}

updatercd_prerm() {
}

updatercd_postrm() {
}


def update_rc_after_parse(d):
    if d.getVar('INITSCRIPT_PACKAGES') == None:
        if d.getVar('INITSCRIPT_NAME') == None:
            raise bb.build.FuncFailed, "%s inherits update-rc.d but doesn't set INITSCRIPT_NAME" % d.getVar('FILE')
        if d.getVar('INITSCRIPT_PARAMS') == None:
            raise bb.build.FuncFailed, "%s inherits update-rc.d but doesn't set INITSCRIPT_PARAMS" % d.getVar('FILE')

python __anonymous() {
    update_rc_after_parse(d)
}

python populate_packages_prepend () {
}
