#!/bin/bash

if [ -a /etc/hydraip-devid ]
	then
		echo "use 100MBit/s"
		ethtool -s eth0 advertise 0xF
		ethtool -s eth1 advertise 0xF
	else
		echo "use 1000MBit/s"
		ethtool -s eth0 advertise 0xFF
		ethtool -s eth1 advertise 0xFF
fi	

