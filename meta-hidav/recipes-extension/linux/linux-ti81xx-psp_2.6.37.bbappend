COMPATIBLE_MACHINE = "hidav"

DEPENDS += " lzop-native "

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " git://git.c3sl.ufpr.br/aufs/aufs2-standalone.git;branch=aufs2.2-37;protocol=git;destsuffix=aufs;name=aufs;rev=4323b3a87dde5b9e2364cd7e51412055dd778a34 \
                   file://hidav-flash-partition-settings.patch \
                   file://btrfs-kobject-include.patch \ 
                   "

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
