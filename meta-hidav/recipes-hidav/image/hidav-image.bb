require hydraip-image.inc

PR = "r3"

export IMAGE_BASENAME = "hydraip-image"

IMAGE_FSTYPES = "tar.bz2 squashfs"

# SDK
IMAGE_INSTALL += " \
  rootfs-overlay \
"
