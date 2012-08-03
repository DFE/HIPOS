#!/usr/bin/python
#
# HidaV automated nas test
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Eik Binschek <binschek@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

import sys
import time
import unittest
import device

from test_samba import TestSamba
from test_webdav import TestWebDAV
from test_nfs import TestNFS

class update:
    dev = device.Device( devtype = "hidav" )

    print "Waiting for Networking to come up..."
    max_wait=120
    while not dev.conn.has_networking():
        time.sleep(1)
        print ("wait {0}s".format(max_wait))
        sys.stdout.flush()
        max_wait -= 1
        if max_wait == 0:
            break
    print "Update: package index"
    dev.update_package_index()
    print "Connecting to device..."
    print "Firmware version: %s" % dev.firmware_version

    print "Boot config:"
    print "  Kernel:      /dev/mtd%s" % dev.bootconfig["kernel"]
    print "  rootfs: /dev/blockdev%s" % dev.bootconfig["rootfs"]
    print "  epoch :             #%s" % dev.bootconfig["epoch"]
    print "Update: rootfs"
    dev.install_package( "hydraip-image-pkg" )
    print "Boot config:"
    print "  Kernel:      /dev/mtd%s" % dev.bootconfig["kernel"]
    print "  rootfs: /dev/blockdev%s" % dev.bootconfig["rootfs"]
    print "  epoch :             #%s" % dev.bootconfig["epoch"]
    print "Update: kernel"
    dev.install_package( "kernel-image-2.6.37" )
    print "Boot config:"
    print "  Kernel:      /dev/mtd%s" % dev.bootconfig["kernel"]
    print "  rootfs: /dev/blockdev%s" % dev.bootconfig["rootfs"]
    print "  epoch :             #%s" % dev.bootconfig["epoch"]
    print "reboot ..."
    dev.conn._serial.reboot( sync=True )
        
if __name__ == '__main__':

    def standalone():
        if len(sys.argv) == 1:
            update()
            print "start tests..."
            unittest.main()
        else:
            print("usage: {0}".format(sys.argv[0]))
            sys.exit(2)
        sys.exit(0)
    standalone()

