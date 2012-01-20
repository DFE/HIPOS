# common stuff for all HydraIP images

require recipes-images/angstrom/console-image.bb

PR_append = "+r8"

IMAGE_INSTALL += " \
  kernel-modules \
  htop \
  bash \
"

# profiling
IMAGE_INSTALL += " \
  systemd-analyze \
"

# helpers
IMAGE_INSTALL += " \
  vim \
  mtd-utils \
  mc \
  nano \  
"

