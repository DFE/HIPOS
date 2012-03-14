DESCRIPTION = "hydraip rootfs pkg"

LICENSE = "GPLv2"

DEPENDS = "hydraip-image"
RDEPENDS = "mtd-utils"
RRECOMMENDS = "kernel"

PACKAGES = "${PN}"
PR = "r1"

FILES_${PN} = "/tmp/hydraip-image-hidav.squashfs"

do_install() {
	install -d ${D}/tmp
	install -m644 ${DEPLOY_DIR_IMAGE}/hydraip-image-hidav.squashfs ${D}/tmp/hydraip-image-hidav.squashfs
}

pkg_postinst_${PN} () {
#!/bin/sh -e

	if [ x"$D" = "x" ]; then
                # flash rootfs
                echo "flash to /dev/mtd5 ..."
                flash_erase /dev/mtd5 0 0 && nandwrite -m -p /dev/mtd5 /tmp/hydraip-image-hidav.squashfs
                echo "clean $D/tmp/hydraip-image-hidav.squashfs"
                rm $D/tmp/hydraip-image-hidav.squashfs
	else
		echo "do not flash ..."
		exit 0
	fi
}

pkg_prerm_${PN} () {
#!/bin/sh -e
	mkdir -p $D/tmp
	touch $D/tmp/hydraip-image-hidav.squashfs
}

do_configure[noexec] = "1"

