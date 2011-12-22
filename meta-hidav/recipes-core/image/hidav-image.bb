require hydraip-image.inc

PR = "r4"

IMAGE_FSTYPES = "tar.bz2 squashfs"

# SDK
IMAGE_INSTALL += " \
  rootfs-overlay \
"
