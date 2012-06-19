PR_append = "+r2"

#
# MTD dev package
#

FILES_${PN}-staticdev += "          \
    ${includedir}/mtd/libmtd.h      \
"

do_install_append() {
    # Texas Instruments MTD drivers are broken for subpage access, as can be pointed out
    # by mtd_subpagetest.ko of the MTD test tools. See e.g. 
    # <http://e2e.ti.com/support/dsp/sitara_arm174_microprocessors/f/416/t/115399.aspx#553136>
    #
    # HidaV provides wrapper scripts for ubi tools to make sure we always access the whole 2k page.
    # This iis a rather crude and unsafe hack and it hurts performance
    # but also makes sure things work as long as TI won't fix their broken drivers.



    for ubitool in ubiformat ubiattach ubinize ; do

        mv ${D}${sbindir}/$ubitool ${D}${sbindir}/$ubitool.real

        echo '#!/bin/sh' > ${D}${sbindir}/$ubitool
        echo '# Wrapper script to force UBI tools to do whole page acces.' >> ${D}${sbindir}/$ubitool
        echo '# TI NAND driver is broken for subpage access.' >> ${D}${sbindir}/$ubitool
        echo '' >> ${D}${sbindir}/$ubitool
        echo 'exec ${0}.real -O 2048 $@' >> ${D}${sbindir}/$ubitool

        chmod 755 ${D}${sbindir}/$ubitool

    done


    # MTD dev package:
    # install libmtd and its headers

    install -d ${D}${includedir}/mtd/
    install -m 0644 ${S}include/libmtd.h ${D}${includedir}/mtd/
    install -d ${D}${libdir}/
    install -m 0644 ${S}/lib/libmtd.a ${D}${libdir}/
}

