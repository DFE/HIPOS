COMPATIBLE_MACHINE = "hidav"

DEPENDS += " lzop-native "

RDEPENDS += " mtd-utils gawk busybox "

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " git://git.c3sl.ufpr.br/aufs/aufs2-standalone.git;branch=aufs2.2-37;protocol=git;destsuffix=aufs;name=aufs;rev=4323b3a87dde5b9e2364cd7e51412055dd778a34 \
                   file://hidav-flash-partition-settings-ti814x.patch \
		   		   file://hidav-flash-partition-settings-ti816x.patch \
                   file://btrfs-kobject-include.patch \ 
                   "

SRCREV = "b3e6c9fd4de6d5d450d6c2235024c4b48160bdad"

MACHINE_KERNEL_PR = "r30"

do_compileconfigs_prepend() {
  cp -r ${WORKDIR}/aufs/Documentation ${S}
  cp -r ${WORKDIR}/aufs/fs ${S}
  cp ${WORKDIR}/aufs/include/linux/aufs_type.h ${S}/include/linux/

  cd ${S}
  patch -p1 < ${WORKDIR}/aufs/aufs2-kbuild.patch
  patch -p1 < ${WORKDIR}/aufs/aufs2-base.patch
  patch -p1 < ${WORKDIR}/aufs/proc_map.patch
  patch -p1 < ${WORKDIR}/aufs/aufs2-standalone.patch
}

pkg_postinst_append() {

  # don't run flash utils on image install
  if [ "x$D" != "x" ]; then
    exit 1	
  fi

  cat /proc/cmdline | awk '{ print $3 }' | grep mmcblk
  if [ $? -eq 0  ]; then
     echo skip nand flash write on sdcard boot
     exit 0
  fi

  cp /boot/uImage /run/uImage_system
  mtd_debug read /dev/mtd2 0 `ls -l /run/uImage_system | awk '{ printf("%s",$5) }'` /run/uImage_flash
  flash_md5sum="`md5sum /run/uImage_flash | awk '{ printf("%s",$1) }'`"
  system_md5sum="`md5sum /run/uImage_system | awk '{ printf("%s",$1) }'`"
  rm /run/uImage_system /run/uImage_flash

  if [ "$flash_md5sum" != "$system_md5sum" ]; then
    # flash kernel
      echo "flash kernel to /dev/mtd2 ..."
      flash_erase /dev/mtd2 0 0 && nandwrite -m -p /dev/mtd2 /boot/uImage
    exit $?
  else
      echo "no new kernel in /boot/uImage"
    exit 0
  fi

}

