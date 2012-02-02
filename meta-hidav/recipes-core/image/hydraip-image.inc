# common stuff for all HydraIP images

require recipes-images/angstrom/console-image.bb

PR_append = "+r9"

IMAGE_INSTALL += " \
  kernel-modules \
  htop \
  bash \
  netbase-systemd \
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

