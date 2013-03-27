#
# Workaround for deprecated ntpdate.bbappend (4.2.6p3) in
# meta-openembedded/meta-systemd/meta-oe/recipes-support/ntp/ntp_4.2.6p3.bbappend
#


require recipes-support/ntp/ntp.inc

EXTRA_OECONF += "--without-openssl --without-crypto"

