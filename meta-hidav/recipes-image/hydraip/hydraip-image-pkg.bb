DESCRIPTION = "hydraip rootfs pkg"

LICENSE = "GPLv2"

DEPENDS = "hydraip-image"
RDEPENDS = "mtd-utils"
RRECOMMENDS = "kernel"

PACKAGES = "${PN}"
PR = "r7"

FILES_${PN} = "/tmp/hydraip-image-hidav.squashfs"

do_install() {
	install -d ${D}/tmp
	install -m644 ${DEPLOY_DIR_IMAGE}/hydraip-image-${MACHINE}.squashfs ${D}/tmp/hydraip-image-hidav.squashfs
}

pkg_postinst_${PN} () {
  # don't run flash utils on image install
  if [ "x$D" != "x" ]; then
    echo "do not flash ..."
    exit 0	# don't give an error code on image install
  fi

  cat /proc/cmdline | awk '{ print $3 }' | grep mmcblk
  if [ $? -eq 0  ]; then
     echo "skip write to nandflash on sdcard boot"
     echo "copy $D/tmp/hydraip-image-hidav.squashfs to /dev/mmcblk0p1"
     mkdir -p /tmp/boot
     mount /dev/mmcblk0p1 /tmp/boot/
     rm /tmp/boot/hidav-root-fs.squashfs
     cp /tmp/hydraip-image-hidav.squashfs /tmp/boot/hidav-root-fs.squashfs
     umount /tmp/boot/
     echo "clean $D/tmp/hydraip-image-hidav.squashfs"
     rm $D/tmp/hydraip-image-hidav.squashfs 
     exit 0
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

  # check bootconfig
  if [ "$mtd_to_write" = "/dev/mtd4" ]; then
	cat /proc/cmdline | grep blockrom4>/dev/null
	if [ "`echo $?`" = "0" ]; then
		echo "error: do not write on used $mtd_to_write"
		echo "check bootconfig"
		echo "skip rootfs update on flash"
        	exit 0
	fi
  fi 
  if [ "$mtd_to_write" = "/dev/mtd5" ]; then
        cat /proc/cmdline | grep blockrom5>/dev/null
        if [ "`echo $?`" = "0" ]; then
                echo "error: do not write on used $mtd_to_write"
		echo "check bootconfig"
                echo "skip rootfs update on flash"
                exit 0
        fi
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
do_install[depends] = "hydraip-image:do_rootfs"

