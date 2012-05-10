COMPATIBLE_MACHINE = "hidav"

DEPENDS += " lzop-native "

RDEPENDS += " mtd-utils gawk busybox bootconfig "

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

TAG = "v2.6.37_TI814XPSP_04.01.00.06.patch2"

SRC_URI = "git://arago-project.org/git/projects/linux-omap3.git;protocol=git;tag=${TAG} \
           file://0001-ti814x-added-code-for-disabling-the-least-significan.patch \
           file://defconfig \
           file://configs \
           file://src/ \
           file://tests/ \
"

SRC_URI_append = " git://git.c3sl.ufpr.br/aufs/aufs2-standalone.git;branch=aufs2.2-37;protocol=git;destsuffix=aufs;name=aufs;rev=c3fc5bd123a94fcfe9bb1aa2fd5f41b16ea7ac04 \
                   file://hidav-flash-partition-settings-ti814x.patch \
                   file://hidav-flash-partition-settings-ti816x.patch \
                   file://btrfs-kobject-include.patch \ 
                   file://mtd-blockrom-glue.patch \ 
                   "

MACHINE_KERNEL_PR = "r48"

# this actually should be do_patch_append, but doing so triggers a syntax error in openembedded
# so we insert it manually.
do_setup_additional_sources() {
  cp -r ${WORKDIR}/aufs/Documentation ${S}
  cp -r ${WORKDIR}/aufs/fs ${S}
  cp ${WORKDIR}/aufs/include/linux/aufs_type.h ${S}/include/linux/

  cd ${S}
  patch -p1 < ${WORKDIR}/aufs/aufs2-kbuild.patch
  patch -p1 < ${WORKDIR}/aufs/aufs2-base.patch
  patch -p1 < ${WORKDIR}/aufs/proc_map.patch
  patch -p1 < ${WORKDIR}/aufs/aufs2-standalone.patch

  cp ${WORKDIR}/src/src/drivers/mtd/blockrom.c ${S}/drivers/mtd
}
addtask setup_additional_sources after do_patch before do_configure

# Create ti816x config on the fly from our defconfig (ti8148).
do_create_ti816x_conf() {
    mkdir -p ${WORKDIR}/configs
    cp -f ${WORKDIR}/defconfig ${WORKDIR}/configs/ti816x
    sed -i -e 's/^CONFIG_ARCH_TI814X=y/# CONFIG_ARCH_TI814X is not set/' ${WORKDIR}/configs/ti816x
    sed -i -e 's/^# CONFIG_ARCH_TI816X is not set/CONFIG_ARCH_TI816X=y/' ${WORKDIR}/configs/ti816x
    sed -i -e 's/^CONFIG_MACH_TI8148EVM=y/CONFIG_MACH_TI8168EVM=y/' ${WORKDIR}/configs/ti816x
}
addtask create_ti816x_conf after do_setup_additional_sources before do_configure

do_make_check_blockrom() {
    # unit tests stage for HidaV blockrom kernel MTD FTL
    
    cp ${WORKDIR}/src/src/drivers/mtd/blockrom.c ${WORKDIR}/tests
    cd ${WORKDIR}/tests
    oe_runmake
}
addtask make_check_blockrom after do_configure before do_compile

do_compile_prepend() {
    # The TI layer can break the kernel's do_compile when you provide
    # your own defconfig and then do a multi-kernel build:
    # Your defconfig kernel will never be built. Instead, one of the
    # "multi-config" kernels will be built in place of the defconfig one.
    #
    # Since I can't quite determine *where* exactly this happens in the meta-ti layer's
    # multi-kernel.inc or linux.inc (they pretty much do everything by themselves, 
    # reinventing the wheel, not using openembedded stages the traditional way, it's quite a mess)
    # I just restore our defconfig just before compilation to even things out.

    cp ${WORKDIR}/defconfig ${S}/.config
    oe_runmake oldconfig
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

  mtd_to_write="/dev/null"
  bootconfig | grep kernel | grep 2 >/dev/null
  if [ "`echo $?`" = "0"  ]; then
	mtd_to_write="/dev/mtd3"
  fi
  bootconfig | grep kernel | grep 3 >/dev/null
  if [ "`echo $?`" = "0"  ]; then
        mtd_to_write="/dev/mtd2"
  fi
  if [ "$mtd_to_write" = "/dev/null" ]; then
	echo "error: active kernel not found"
	echo "skip kernel update on flash"
	exit 0
  fi

  cp /boot/uImage /run/uImage_system
  mtd_debug read $mtd_to_write 0 `ls -l /run/uImage_system | awk '{ printf("%s",$5) }'` /run/uImage_flash
  flash_md5sum="`md5sum /run/uImage_flash | awk '{ printf("%s",$1) }'`"
  system_md5sum="`md5sum /run/uImage_system | awk '{ printf("%s",$1) }'`"
  rm /run/uImage_system /run/uImage_flash

  if [ "$flash_md5sum" != "$system_md5sum" ]; then
    # flash kernel
      echo "flash kernel to $mtd_to_write ..."
      flash_erase $mtd_to_write 0 0 && nandwrite -m -p $mtd_to_write /boot/uImage
  else
      echo "no new kernel in /boot/uImage"
    exit 0
  fi

  if [ "$mtd_to_write" = "/dev/mtd2" ]; then
    bootconfig set-kernel mtd2
  fi
  if [ "$mtd_to_write" = "/dev/mtd3" ]; then
    bootconfig set-kernel mtd3
  fi

}

