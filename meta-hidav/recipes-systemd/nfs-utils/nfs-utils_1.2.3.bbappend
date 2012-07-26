# bbapped to openembedded-core/meta/recipes-connectivity/nfs-utils/...

inherit systemd

PR_append = "+r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://nfsd.service                  \
                   file://rpc.idmapd.service            \
                   file://rpc.mountd.service            \
                   file://rpc.statd.service             \
                   file://sm-notify.service             \
                   file://var-lib-nfs-rpc_pipefs.mount  \
                  "

# systemd
PACKAGES =+ "${PN}-systemd"

FILES_${PN}-systemd += "${base_libdir}/systemd"
RDEPENDS_${PN}-systemd += "${PN}"

SYSTEMD_PACKAGES = "${PN}-systemd"
SYSTEMD_SERVICE = "nfsd.service rpc.idmapd.service rpc.mountd.service rpc.statd.service sm-notify.service "

do_install_append () {
  install -d ${D}${base_libdir}/systemd/system

  for service in $SYSTEMD_SERVICE; do 
    install -m 0644 ${WORKDIR}/$service ${D}${base_libdir}/systemd/system/
  done
  install -m 0644 ${WORKDIR}/var-lib-nfs-rpc_pipefs.mount ${D}${base_libdir}/systemd/system/
}


