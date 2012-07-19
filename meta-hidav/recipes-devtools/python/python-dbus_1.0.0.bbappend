
#
# API doc generation requires epydoc which is not present in OE but is
# sometimes (wrongly) detected on the build host (i.e. outside the build 
# sandbox). This false detection will occasionally break the build so we 
# disable API doc generation completely.

EXTRA_OECONF += " --disable-api-docs "
PR_append = "+r1"
