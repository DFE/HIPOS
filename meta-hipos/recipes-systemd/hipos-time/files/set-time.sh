#!/bin/bash
TIME=`/usr/bin/drbcc --dev /dev/serial/by-path/platform-orion-ehci.0-usb-0:1.4:1.1-port0 --cmd=getrtc | awk '{ print $2 " " $3 }'`
date -u -s "$TIME"
