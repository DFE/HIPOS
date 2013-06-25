require hydraip-image.inc

PR_append = ".9"

export IMAGE_BASENAME = "hydraip-image"

IMAGE_FSTYPES = "tar.bz2 squashfs"

#IMAGE_INSTALL += " \
#  rootfs-overlay rootfs-overlay-systemd rootfs-overlay-tools \
#"

addtask package_write after do_rootfs before do_build

