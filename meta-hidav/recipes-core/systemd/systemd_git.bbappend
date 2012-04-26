# do not generate rc-links
PR_append = "+r1"
DEPENDS += " grep "

do_install_append() {
	if ! grep "Restart=" ${D}/lib/systemd/system/systemd-journald.service ; then
		echo "RestartSec= 1" >>${D}/lib/systemd/system/systemd-journald.service
		echo "Restart= always" >>${D}/lib/systemd/system/systemd-journald.service
	fi
}
