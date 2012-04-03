DESCRIPTION = "hydraip rootfs pkg"

LICENSE = "GPLv2"

DEPENDS = "hydraip-image"
RDEPENDS = "mtd-utils"
RRECOMMENDS = "kernel"

PACKAGES = "${PN}"
PR = "r3"

FILES_${PN} = "/tmp/hydraip-image-hidav.squashfs"

do_install() {
	install -d ${D}/tmp
	install -m644 ${DEPLOY_DIR_IMAGE}/hydraip-image-hidav.squashfs ${D}/tmp/hydraip-image-hidav.squashfs
}

pkg_postinst_${PN} () {
  # don't run flash utils on image install
  if [ "x$D" != "x" ]; then
    echo "do not flash ..."
    exit 0	# don't give an error code on image install
  fi

  mtd_to_write="/dev/null"
  bootconfig | grep rootfs | grep 4 >/dev/null
  if [ "`echo $?`" = "0" ]; then
	mtd_to_write="/dev/mtd5"
  fi
  bootconfig | grep rootfs | grep 5 >/dev/null
  if [ "`echo $?`" = "0" ]; then
        mtd_to_write="/dev/mtd4"
  fi
  if [ "$mtd_to_write" = "/dev/null" ]; then
	echo "error: active rootfs not found"
	echo "skip rootfs update on flash"
	exit 0
  fi

  # flash rootfs
  echo "flash to $mtd_to_write ..."
  flash_erase $mtd_to_write 0 0 && nandwrite -m -p $mtd_to_write /tmp/hydraip-image-hidav.squashfs
  echo "clean $D/tmp/hydraip-image-hidav.squashfs"
  rm $D/tmp/hydraip-image-hidav.squashfs

  if [ "$mtd_to_write" = "/dev/mtd4" ]; then
    bootconfig set-rootfs mtd4
  fi
  if [ "$mtd_to_write" = "/dev/mtd5" ]; then
    bootconfig set-rootfs mtd5
  fi

  exit 0
}

pkg_prerm_${PN} () {
#!/bin/sh -e
	mkdir -p $D/tmp
	touch $D/tmp/hydraip-image-hidav.squashfs
}

do_configure[noexec] = "1"

