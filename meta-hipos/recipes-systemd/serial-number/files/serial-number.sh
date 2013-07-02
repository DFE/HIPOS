#!/bin/bash

if [ -a /etc/hydraip-devid ]
        then
                . /etc/hydraip-devid
fi

if [ -z $serial_number ]
	then
		echo "serial_number=1234567890" >>/etc/hydraip-devid
		. /etc/hydraip-devid
fi

echo "serial_number=$serial_number"

exit 0

