# hidav image

PR = "r2"

LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"

IMAGE_PREPROCESS_COMMAND = "rootfs_update_timestamp"

IMAGE_FSTYPES = "tar.bz2 squashfs"

IMAGE_INSTALL += " \
	angstrom-task-boot \
	task-basic \
	${CONMANPKGS} \
"
CONMANPKGS = "connman connman-plugin-loopback connman-plugin-ethernet connman-plugin-wifi connman-systemd"
CONMANPKGS_libc-uclibc = ""

# IMAGE_DEV_MANAGER   = "udev"
IMAGE_INIT_MANAGER  = "systemd"
IMAGE_INITSCRIPTS   = " "
IMAGE_LOGIN_MANAGER = "tinylogin shadow"

inherit image

export IMAGE_BASENAME = "hydraip-image"

IMAGE_INSTALL += " \
  kernel-modules \
  htop \
  bash \
"

# helpers
IMAGE_INSTALL += " \
  vim \
  mtd-utils \
  mc \
  nano \  
"
