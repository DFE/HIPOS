COMPATIBLE_MACHINE = "hidav"

DEPENDS += " lzop-native "

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += " 	  git://git.c3sl.ufpr.br/aufs/aufs2-standalone.git;branch=aufs2.2-37;protocol=git;destsuffix=aufs;name=aufs" \
                  file://hidav-flash-partition-settings.patch \
                  file://btrfs-kobject-include.patch \ 
                  file://hidav-flash-enable-prefetch.patch "

SRCREV = "${AUTOREV}"
SRCREV_aufs = "${AUTOREV}"

SRCREV_FORMAT = "${AUTOREV}"

MACHINE_KERNEL_PR_append = ""

do_configure_prepend() {
  cp -r ${WORKDIR}/aufs/Documentation ${S}
  cp -r ${WORKDIR}/aufs/fs ${S}
  cp ${WORKDIR}/aufs/include/linux/aufs_type.h ${S}/include/linux/

  cd ${S}
  patch -p1 < ${WORKDIR}/aufs/aufs2-kbuild.patch
  patch -p1 < ${WORKDIR}/aufs/aufs2-base.patch
  patch -p1 < ${WORKDIR}/aufs/proc_map.patch
  patch -p1 < ${WORKDIR}/aufs/aufs2-standalone.patch
}
