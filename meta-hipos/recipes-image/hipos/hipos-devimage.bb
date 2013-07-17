require hipos-image.inc

PR_append = ".1"

export IMAGE_BASENAME = "hipos-devimage"

IMAGE_FSTYPES = "tar.bz2"

# SDK
IMAGE_INSTALL += " \
  task-native-sdk \
  gdb \
  gdbserver \
  openssh-sftp-server \
  subversion \
  git \
"

# utils 
IMAGE_INSTALL += " \
  mmc-utils \
"
