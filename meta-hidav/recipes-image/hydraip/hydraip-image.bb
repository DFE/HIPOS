require hydraip-image.inc

PR_append = ".7"

export IMAGE_BASENAME = "hydraip-image"

IMAGE_FSTYPES = "tar.bz2 squashfs"

IMAGE_INSTALL_append_hidav += " \
  rootfs-overlay rootfs-overlay-systemd rootfs-overlay-tools \
"

