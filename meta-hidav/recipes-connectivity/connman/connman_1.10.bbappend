FILESEXTRAPATHS_prepend := "${THISDIR}/connman:"
PR_append = "+r0"
SRC_URI += " file://0001-connman-do-not-allow-connman-to-be-DNSPROXY.patch"
