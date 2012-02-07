COMPATIBLE_MACHINE = "hidav"

DEPENDS += " lzop-native "

RDEPENDS += " mtd-utils gawk busybox "

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " git://git.c3sl.ufpr.br/aufs/aufs2-standalone.git;branch=aufs2.2-37;protocol=git;destsuffix=aufs;name=aufs;rev=4323b3a87dde5b9e2364cd7e51412055dd778a34 \
                   file://hidav-flash-partition-settings-ti814x.patch \
		   		   file://hidav-flash-partition-settings-ti816x.patch \
                   file://btrfs-kobject-include.patch \ 
                   "

SRCREV = "757600ad1177e0b8a0c8ef48ea449470b3efae64"

MACHINE_KERNEL_PR = "r26"

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

  cp /boot/uImage /run/uImage_system
  mtd_debug read /dev/mtd1 0 `ls -l /run/uImage_system | awk '{ printf("%s",$5) }'` /run/uImage_flash
  flash_md5sum="`md5sum /run/uImage_flash | awk '{ printf("%s",$1) }'`"
  system_md5sum="`md5sum /run/uImage_system | awk '{ printf("%s",$1) }'`"
  rm /run/uImage_system /run/uImage_flash

  if [ "$flash_md5sum" != "$system_md5sum" ]; then
    # flash kernel
      flash_erase /dev/mtd1 0 0 && nandwrite -m -p /dev/mtd1 /boot/uImage
    exit $?
  else
    exit 0
  fi

}

