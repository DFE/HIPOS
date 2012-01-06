require hydraip-image.inc

PR = "${INC_PR}.1"

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
