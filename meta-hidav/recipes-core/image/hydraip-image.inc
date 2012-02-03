# common stuff for all HydraIP images

require recipes-images/angstrom/systemd-image.bb

PR_append = "+r11"

IMAGE_INSTALL += " \
  kernel-modules \
  htop \
  bash \
"

# systemd
IMAGE_INSTALL += " \
  portmap-systemd \
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

# system / plumbing level tools
IMAGE_INSTALL += "  \
    ethtool         \
"

