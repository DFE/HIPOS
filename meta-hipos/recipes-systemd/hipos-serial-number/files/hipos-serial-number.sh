#!/bin/bash

/usr/bin/drbcc --dev /dev/serial/by-path/platform-orion-ehci.0-usb-0:1.4:1.1-port0 --cmd="gfiletype 0x50,/tmp/hydraip-devid"

diff /tmp/hydraip-devid /etc/hipos/hydraip-devid

if [ $? -ne 0  ]
	then
		cp /tmp/hydraip-devid /etc/hipos/hydraip-devid
		touch /etc/hipos/hydraip-devid
		cd /etc
		rm /etc/hydraip-devid
		ln -s /etc/hipos/hydraip-devid hydraip-devid
fi

if [ -a /etc/hipos/hydraip-devid ]
        then
                . /etc/hipos/hydraip-devid
fi

if [ -z $serial_number ]
	then
		echo "warning: no serial number available"
fi

echo "serial_number=$serial_number"

exit 0

