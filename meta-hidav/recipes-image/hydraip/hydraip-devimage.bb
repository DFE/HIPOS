require hydraip-image.inc

PR_append = ".4"

export IMAGE_BASENAME = "hydraip-devimage"

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

# DNS: deactivated temporary on build issues
IMAGE_INSTALL += "openjdk-6-jdk"
