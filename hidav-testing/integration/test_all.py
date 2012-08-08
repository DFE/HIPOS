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

""" Tool to start all available test on a HidaV-device """

import sys
import time
import unittest
import device

from test_samba import TestSamba
from test_webdav import TestWebDAV
from test_nfs import TestNFS
from test_http import TestHTTP

class Test(object):

    def __init__(self):
        """ This function prepare HidaV-Device """

        if len(sys.argv) == 1:
            self.mydev = MyDevice()
            self.mydev.update()
            self.mydev.mount()
            self.test_all()
        else:
            print("usage: {0}".format(sys.argv[0]))
            sys.exit(2)
        sys.exit(0)

    def test_all(self):
        """ This function start all testes """
        
        print "start tests..."
        unittest.main()
        sys.exit(0)

class MyDevice(object):
    def __init__(self):
        """ boot HidaV-divice to NAND """
        self.dev = device.Device( devtype = "hidav" )
    
        print "Waiting for Networking to come up..."
        max_wait = 120
        while not self.dev.conn.has_networking():
            time.sleep(1)
            print ("wait {0}s".format(max_wait))
            sys.stdout.flush()
            max_wait -= 1
            if max_wait == 0:
                break
        print "Boot to NAND ..."
        self.dev.conn._serial.boot_to_nand(sync = True,
                                      kernel_partition = None,
                                      rootfs_partition = None )
        
    def update(self):
        """ This function install ketnel and roootfs to a HidaV-divice """
    
        print "Waiting for Networking to come up..."
        max_wait = 120
        while not self.dev.conn.has_networking():
            time.sleep(1)
            print ("wait {0}s".format(max_wait))
            sys.stdout.flush()
            max_wait -= 1
            if max_wait == 0:
                break
        print "Update: package index"
        self.dev.update_package_index()
        print "Connecting to device..."
        print "Firmware version: %s" % self.dev.firmware_version
    
        print "Boot config:"
        print "  Kernel:      /dev/mtd%s" % self.dev.bootconfig["kernel"]
        print "  rootfs: /dev/blockdev%s" % self.dev.bootconfig["rootfs"]
        print "  epoch :             #%s" % self.dev.bootconfig["epoch"]
        print "Update: rootfs"
        self.dev.install_package( "hydraip-image-pkg" )
        print "Boot config:"
        print "  Kernel:      /dev/mtd%s" % self.dev.bootconfig["kernel"]
        print "  rootfs: /dev/blockdev%s" % self.dev.bootconfig["rootfs"]
        print "  epoch :             #%s" % self.dev.bootconfig["epoch"]
        print "Update: kernel"
        self.dev.install_package( "kernel-image-2.6.37" )
        print "Boot config:"
        print "  Kernel:      /dev/mtd%s" % self.dev.bootconfig["kernel"]
        print "  rootfs: /dev/blockdev%s" % self.dev.bootconfig["rootfs"]
        print "  epoch :             #%s" % self.dev.bootconfig["epoch"]
        print "reboot ..."
        print "Boot to NAND ..."
        self.dev.conn._serial.boot_to_nand(sync = True,
                                      kernel_partition = None,
                                      rootfs_partition = None )

    def __format__(self):
        """ This function mount /dev/sda1 on a HidaV-divice """

        retc, msg = self.dev.conn.cmd( "mount | grep /dev/sda1" )
        if retc == 0:
            return
        retc, msg = self.dev.conn.cmd( "ls /dev/sda" )
        if retc != 0:
            print "error: /dev/sda not found"
            sys.exit(255)
        retc, msg = self.dev.conn.cmd( "ls /dev/sda1" )
        if retc != 0:
            print "Creating Partition"
            retc, msg = self.dev.conn.cmd( """fdisk /dev/sda << EOF
o
n
p
1


w
EOF
""" )
            print msg
            if retc != 0:
                print "error: Creating Partition failed"
                sys.exit(255)
            print "make ext4 filesystem on /dev/sda1"
            self.dev.update_package_index()
            self.dev.install_package( "e2fsprogs-mke2fs" )
            retc, msg = self.dev.conn.cmd( "mke2fs -t ext4 /dev/sda1" )
            print msg
            if retc != 0:
                print "error: make ext4 filesystem"
                sys.exit(255)
        retc, msg = self.dev.conn.cmd( "ls -l /media/sda1" )
        if retc != 0:
            retc, msg = self.dev.conn.cmd( "mkdir -p /media/sda1" )
            print msg
            if retc != 0:
                print "error: mkdir -p /media/sda1 failed"
                sys.exit(255)
                
    def mount(self):
        """ This function format /dev/sda1 on a HidaV-divice """

        print "Waiting for Networking to come up..."
        max_wait = 120
        while not self.dev.conn.has_networking():
            time.sleep(1)
            print ("wait {0}s".format(max_wait))
            sys.stdout.flush()
            max_wait -= 1
            if max_wait == 0:
                break
        self.__format__(self)
        print "mount /dev/sda1"
        retc, msg = self.dev.conn.cmd( "mount /dev/sda1 /media/sda1" )
        if retc != 0:
            print msg
            print "make ext4 filesystem on /dev/sda1"
            self.dev.update_package_index()
            self.dev.install_package( "e2fsprogs-mke2fs" )
            retc, msg = self.dev.conn.cmd( "mke2fs -t ext4 /dev/sda1" )
            print msg
            if retc != 0:
                print "error: make ext4 filesystem"
                sys.exit(255)
        retc, msg = self.dev.conn.cmd( "mount /dev/sda1 /media/sda1" )
        if retc != 0:
            print msg
            print "error: mount /dev/sda1 failed"
            sys.exit(255)
        retc, msg = self.dev.conn.cmd( "grep /dev/sda1 /etc/fstab" )
        if retc != 0:
            retc, msg = self.dev.conn.cmd( 'echo "/dev/sda1            ' \
                 + '/media/sda1 ext4       defaults,errors=remount-ro' \
                 + ',noatime	0	2">>/etc/fstab ' )
            if retc != 0:
                print "error: unable to add /dev/sda1 to /etc/fstab"
                sys.exit(255)


if __name__ == '__main__':

    def standalone():
        mytest = Test()
        
    standalone()

