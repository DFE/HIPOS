# common stuff for all HydraIP images

require recipes-images/angstrom/systemd-image.bb

PR_append = "+r20"

IMAGE_FEATURES += " package-management "

IMAGE_INSTALL += " \
  kernel-modules \
  htop \
  bash \
  portmap portmap-systemd \
  bootconfig \
  healthy \
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
    ltrace \
"

# nfs support
IMAGE_INSTALL += "  \
    nfs-utils        \
    nfs-utils-client \
    nfs-utils-doc    \
    nfs-utils-stats  \
    rpcbind  \
    quota  \
"

