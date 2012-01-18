# common stuff for all HydraIP images

require recipes-images/angstrom/console-image.bb

PR_append = "+r6"

IMAGE_INSTALL += " \
  kernel-modules \
  htop \
  bash \
"

# systemd-analyze depends
IMAGE_INSTALL += " \
  python \
  python-dbus \
  python-pycairo \
"

# helpers
IMAGE_INSTALL += " \
  vim \
  mtd-utils \
  mc \
  nano \  
"

# SDK
IMAGE_INSTALL_append_hidav += " \
  rootfs-overlay \
"

