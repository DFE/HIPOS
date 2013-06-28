#!/bin/bash

if [ -a /etc/hydraip-devid ]
        then
                . /etc/hydraip-devid
        else
                echo "use 100MBit/s"
                lanspeed=ff
fi

echo "lanspeed= $lanspeed"

if [ $lanspeed = "gg" ]
        then
                echo "use 1000MBit/s"
                ethtool -s eth0 advertise 0xFF
                ethtool -s eth1 advertise 0xFF
        else
                echo "use 100MBit/s"
                ethtool -s eth0 advertise 0xF
                ethtool -s eth1 advertise 0xF
fi

