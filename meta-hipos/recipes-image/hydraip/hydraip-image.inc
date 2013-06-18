# common stuff for all HydraIP images

require recipes-images/angstrom/systemd-image.bb

PR_append = "+r27"

IMAGE_FEATURES += " package-management "

IMAGE_INSTALL += "          \
  kernel-modules            \
  htop                      \
  bash                      \
  bootconfig                \
  healthy                   \
"

# profiling
IMAGE_INSTALL += "  \
  systemd-analyze   \
"

# helpers
IMAGE_INSTALL += "  \
  vim               \
  mtd-utils         \
  mc                \
  nano              \
  screen            \
"

# system / plumbing level tools
IMAGE_INSTALL += "  \
    ethtool         \
    ltrace          \
"

# nfs support
IMAGE_INSTALL += "      \
    nfs-utils           \
    nfs-utils-systemd   \
    nfs-utils-client    \
    nfs-utils-doc       \
    nfs-utils-stats     \
    rpcbind             \
    rpcbind-systemd     \
    quota               \
    quota-systemd       \
"

# smb support
IMAGE_INSTALL += " samba-systemd "

# http/webdav support
IMAGE_INSTALL += "         \
    lighttpd               \
    lighttpd-module-webdav \
    lighttpd-module-alias  \
"

# iscsi support
IMAGE_INSTALL += "              \
    iscsi-target                \
    iscsi-target-systemd        \
    open-iscsi-kernel           \
    open-iscsi-user             \
    open-iscsi-user-systemd     \
"

# file system tools
IMAGE_INSTALL += "      \
    e2fsprogs           \
    e2fsprogs-mke2fs    \
    parted              \
    mdadm               \
"

