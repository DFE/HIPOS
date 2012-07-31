# bbapped to meta-openembedded/meta-oe/recipes-connectivity/samba/samba_3.5.6.bb

inherit systemd

PR_append = "+r3"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://samba.service \
                   file://netbios.service "

# systemd
PACKAGES =+ "${PN}-systemd"

FILES_${PN}-systemd += " ${base_libdir}/systemd "

FILES_${PN} += " /usr/lib/pdb \
		 /usr/lib/gpext \
		 /usr/lib/rpc \
		 /usr/lib/idmap \
		 /usr/lib/nss_info \
		 /usr/lib/perfcount "

RDEPENDS_${PN}-systemd += "${PN}"

SYSTEMD_PACKAGES = "${PN}-systemd"

SYSTEMD_SERVICE = "samba.service netbios.service "

do_install_append () {
  install -d ${D}${base_libdir}/systemd/system

  for service in $SYSTEMD_SERVICE; do
    install -m 0644 ${WORKDIR}/$service ${D}${base_libdir}/systemd/system/
  done

}

pkg_postinst_${PN} () {

if grep "\[pub\]" /etc/samba/smb.conf 2>&1 >/dev/null; then
exit 0
fi

mkdir -p /mnt/pub
chmod 777 /mnt/pub

echo "

[pub]
comment = HidaV public share
path = /mnt/pub
public = yes
writable = yes

" >>/etc/samba/smb.conf

}

