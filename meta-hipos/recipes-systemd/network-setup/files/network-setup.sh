#!/bin/bash

if [ -a /etc/hydraip-devid ]
        then
                . /etc/hydraip-devid
        else
                lanspeed=ff
fi

echo "lanspeed=$lanspeed"

if [ $lanspeed = "gg" ]
        then
                echo "use 1000MBit/s"
                ethtool -s eth0 advertise 0xFF
                ethtool -s eth1 advertise 0xFF
                exit 0
fi
if [ $lanspeed = "fg" ]
        then
                echo "eth0: use 100MBit/s"
                ethtool -s eth0 advertise 0xF
                echo "eth1: use 1000MBit/s"
                ethtool -s eth1 advertise 0xFF
                exit 0
fi
if [ $lanspeed = "gf" ]
        then
                echo "eth0: use 1000MBit/s"
                ethtool -s eth0 advertise 0xFF
                echo "eth1: use 100MBit/s"
                ethtool -s eth1 advertise 0xF
                exit 0
fi

echo "use 100MBit/s"
ethtool -s eth0 advertise 0xF
ethtool -s eth1 advertise 0xF

exit 0

