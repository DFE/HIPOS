require hipos-image.inc

PR_append = ".1"

export IMAGE_BASENAME = "hipos-prodimage"

IMAGE_FSTYPES = "tar.bz2 squashfs"

#IMAGE_INSTALL += " \
#  rootfs-overlay rootfs-overlay-systemd rootfs-overlay-tools \
#"

addtask package_write after do_rootfs before do_build
