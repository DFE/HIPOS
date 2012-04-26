#!/usr/bin/python
#
# HidaV automated test framework system upgrade test
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

import sys, time, power

from device import Device

def run_upgrade():
    dev = Device( devtype = "hidav" )

    print "Booting into NAND..."
    dev.boot_to_nand()
    
    print "Waiting for Networking to come up..."
    while not dev.conn.has_networking():
        time.sleep(1)
        sys.stdout.write(".")
    print "\nWe now have networking."
    print "Firmware version: %s" % dev.firmware_version

    print "Installing kernel package..."
    dev.install_package("kernel")

    print "Installing hydraip-image-pkg package..."
    dev.install_package("hydraip-image-pkg")

run_upgrade()
