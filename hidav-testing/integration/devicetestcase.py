#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
"""
Created on Thu Aug  9 09:43:34 2012

    

@author: binschek
"""

import sys
import time
import unittest
import device
import threading

import logger

class DeviceTestCase(unittest.TestCase):

    __dev = None
    __devsem = threading.Lock()
    
    @classmethod
    def get_device(cls):
        if not cls.__dev:
            cls.__devsem.acquire()
            if not cls.__dev:
                cls.__create_device()
            cls.__devsem.release()
        return cls.__dev

    @classmethod        
    def __create_device(cls):
        """ boot HidaV-divice to NAND """
        cls.__dev = device.Device( devtype = "hidav" )
        print "Boot to NAND ..."
        cls.__dev.conn._serial.boot_to_nand(sync = False,
                                      kernel_partition = None,
                                      rootfs_partition = None )

    def __init__(self, *args, **kwargs):
        """ open unit-test """
        super(DeviceTestCase, self).__init__(*args, **kwargs)
        self.dev = DeviceTestCase.get_device()
        self.logger = logger.init()
        
        
    def wait_for_network(self):
        print "Waiting for Networking to come up..."
        max_wait = 120
        while not self.dev.conn.has_networking():
            time.sleep(1)
            print ("wait {0}s".format(max_wait))
            sys.stdout.flush()
            max_wait -= 1
            if max_wait == 0:
                break

    def update(self):
        """ This function install ketnel and roootfs to a HidaV-divice """

        self.wait_for_network()
        print "Boot to SD"
        self.dev.conn._serial.reboot()
        self.wait_for_network()
        print "erase /dev/mtd6"
        retc, msg = self.dev.conn.cmd("flash_erase /dev/mtd6 0 0")
        if retc != 0:
            print msg
            sys.exit(255)
        print "Boot to NAND ..."
        self.dev.conn._serial.boot_to_nand(sync = False,
                                      kernel_partition = None,
                                      rootfs_partition = None )
        self.wait_for_network()

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
        self.dev.conn._serial.boot_to_nand(sync = False,
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

        self.wait_for_network()
        self.__format__()
        print "mount /dev/sda1"
        retc, msg = self.dev.conn.cmd( "mount | grep /dev/sda1" )
        if retc == 0:
            return
        retc, msg = self.dev.conn.cmd( "mount /dev/sda1 /media/sda1" )
        if (retc != 0):
            print msg
            print "make ext4 filesystem on /dev/sda1"
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
        sys.exit(0)
        
    standalone()
