COMPATIBLE_MACHINE = "hidav"

DEPENDS += " lzop-native "

RDEPENDS += " mtd-utils gawk busybox "

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

TAG = "v2.6.37_TI814XPSP_04.01.00.06.patch2"

SRC_URI = "git://arago-project.org/git/projects/linux-omap3.git;protocol=git;tag=${TAG} \
           file://0001-ti814x-added-code-for-disabling-the-least-significan.patch \
           file://defconfig \
		   file://configs \
"

SRC_URI_append = " git://git.c3sl.ufpr.br/aufs/aufs2-standalone.git;branch=aufs2.2-37;protocol=git;destsuffix=aufs;name=aufs;rev=27da2e1df6d7f5bb33bb9d2569bba2fe0407adf7 \
                   file://hidav-flash-partition-settings-ti814x.patch \
		   		   file://hidav-flash-partition-settings-ti816x.patch \
                   file://btrfs-kobject-include.patch \ 
                   "

MACHINE_KERNEL_PR = "r41"

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

pkg_postinst_kernel-image_append() {

  # don't run flash utils on image install
  if [ "x$D" != "x" ]; then
    exit 0	# don't give an error code on image install
  fi

  echo "start postinst append ${KERNEL_VERSION}"

  cat /proc/cmdline | awk '{ print $3 }' | grep mmcblk
  if [ $? -eq 0  ]; then
     echo "skip write to nandflash on sdcard boot"
     echo "copy /boot/uImage-${KERNEL_VERSION} to /dev/mmcblk0p1"
     mkdir -p /tmp/boot
     mount /dev/mmcblk0p1 /tmp/boot/
     cp /boot/uImage-${KERNEL_VERSION} /tmp/boot/uImage
     umount /tmp/boot/
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

