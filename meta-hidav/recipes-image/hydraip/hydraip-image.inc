# common stuff for all HydraIP images

require recipes-images/angstrom/systemd-image.bb

# temporary replace connman with networkmanager till it's more stable
CONMANPKGS = "networkmanager"

PR_append = "+r15"

IMAGE_FEATURES += " package-management "

IMAGE_INSTALL += " \
  kernel-modules \
  htop \
  bash \
  portmap portmap-systemd \
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

