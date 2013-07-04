#!/bin/bash

/usr/bin/drbcc --dev /dev/serial/by-path/platform-orion-ehci.0-usb-0:1.4:1.1-port0 --cmd="gfiletype 0x50,/tmp/hipos-devid"

diff /tmp/hipos-devid /etc/hipos/hipos-devid

if [ $? -ne 0  ]
	then
		cp /tmp/hipos-devid /etc/hipos/hipos-devid
		touch /etc/hipos/hipos-devid
		cd /etc
		rm /etc/hydraip-devid
		ln -s /etc/hipos/hipos-devid hydraip-devid
		echo "hipos-device-info: /etc/hipos/hipos-devid updated"
		cat /etc/hipos/hipos-devid
fi

if [ -a /etc/hipos/hipos-devid ]
        then
                . /etc/hipos/hipos-devid
fi

exit 0

