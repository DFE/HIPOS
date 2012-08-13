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
import unittest
import logger

from test_samba import TestSamba
from test_webdav import TestWebDAV
from test_nfs import TestNFS
from test_http import TestHTTP

from devicetestcase import DeviceTestCase


class TestDevice(object):
    """ class to handle update and mount of HidaV device
    """
    
    def __init__(self, dev):
        """ init Device
        
            :param dev: handle of DeviceTestCase.get_device()
        """
        self.dev = dev
        self.logger = logger.init()
    
    def update_device(self):
        """ update HidaV device
        
            :param dev: devive instance
        """
        
        dev = self.dev
        self.logger.info("Boot to SD")
        dev.wait_for_network()
        self.logger.info("erase /dev/mtd6")
        retc, msg = dev.conn.cmd("flash_erase /dev/mtd6 0 0")
        if retc != 0:
            self.logger.error(msg)
            sys.exit(255)
        self.logger.info("Boot to NAND ...")
        dev.reboot(to_nand=True)
        dev.wait_for_network()
    
        self.logger.info("Connecting to device...")
        self.logger.info("Firmware version: %s" % dev.firmware_version)
    
        self.logger.info("Boot config:")
        self.logger.info("  Kernel:      /dev/mtd%s" 
                        % dev.bootconfig["kernel"])
        self.logger.info("  rootfs: /dev/blockdev%s" 
                        % dev.bootconfig["rootfs"])
        self.logger.info("  epoch :             #%s" 
                        % dev.bootconfig["epoch"])
        self.logger.info("Update: rootfs")
        dev.install_package( "hydraip-image-pkg" )
        self.logger.info("Boot config:")
        self.logger.info("  Kernel:      /dev/mtd%s" 
                        % dev.bootconfig["kernel"])
        self.logger.info("  rootfs: /dev/blockdev%s" 
                        % dev.bootconfig["rootfs"])
        self.logger.info("  epoch :             #%s" 
                        % dev.bootconfig["epoch"])
        self.logger.info("Update: kernel")
        dev.install_package( "kernel-image-2.6.37" )
        self.logger.info("Boot config:")
        self.logger.info("  Kernel:      /dev/mtd%s" 
                        % dev.bootconfig["kernel"])
        self.logger.info("  rootfs: /dev/blockdev%s" 
                        % dev.bootconfig["rootfs"])
        self.logger.info("  epoch :             #%s" 
                        % dev.bootconfig["epoch"])
        self.logger.info("reboot ...")
        self.logger.info("Boot to NAND ...")
        dev.reboot(to_nand=True)
                                    
                                    
    def format_device(self):
        """ format HidaV device
        
            :param dev: devive instance
        """
        
        dev = self.dev
        retc, msg = dev.conn.cmd( "mount | grep /dev/sda1" )
        if retc == 0:
            return
        retc, msg = dev.conn.cmd( "ls /dev/sda" )
        if retc != 0:
            self.logger.error("error: /dev/sda not found")
            sys.exit(255)
        retc, msg = dev.conn.cmd( "ls /dev/sda1" )
        if retc != 0:
            self.logger.warn("Creating Partition")
            retc, msg = dev.conn.cmd( """fdisk /dev/sda << EOF
o
n
p
1


w
EOF
""" )
            self.logger.info(msg)
            if retc != 0:
                self.logger.error("error: Creating Partition failed")
                sys.exit(255)
            self.logger.info("make ext4 filesystem on /dev/sda1")
            dev.install_package( "e2fsprogs-mke2fs" )
            retc, msg = dev.conn.cmd( "mke2fs -t ext4 /dev/sda1" )
            self.logger.info(msg)
            if retc != 0:
                self.logger.error("error: make ext4 filesystem")
                sys.exit(255)
        retc, msg = dev.conn.cmd( "ls -l /media/sda1" )
        if retc != 0:
            retc, msg = dev.conn.cmd( "mkdir -p /media/sda1" )
            self.logger.info(msg)
            if retc != 0:
                self.logger.error("error: mkdir -p /media/sda1 failed")
                sys.exit(255)
                    
    def mount(self):
        """ This function format /dev/sda1 on a HidaV-divice 
        
            :param dev: device instance
        """
        
        dev = self.dev
        dev.wait_for_network()
        self.format_device()
        self.logger.info("mount /dev/sda1")
        retc, msg = dev.conn.cmd( "mount | grep /dev/sda1" )
        if retc == 0:
            return
        retc, msg = dev.conn.cmd( "mount /dev/sda1 /media/sda1" )
        if (retc != 0):
            self.logger.warn(msg)
            self.logger.info("make ext4 filesystem on /dev/sda1")
            dev.install_package( "e2fsprogs-mke2fs" )
            retc, msg = dev.conn.cmd( "mke2fs -t ext4 /dev/sda1" )
            self.logger.info(msg)
            if retc != 0:
                self.logger.error("error: make ext4 filesystem")
                sys.exit(255)
            retc, msg = dev.conn.cmd( "mount /dev/sda1 /media/sda1" )
            if retc != 0:
                self.logger.error(msg)
                self.logger.error("error: mount /dev/sda1 failed")
                sys.exit(255)
        retc, msg = dev.conn.cmd( "grep /dev/sda1 /etc/fstab" )
        if retc != 0:
            retc, msg = dev.conn.cmd( 'echo "/dev/sda1            ' \
                 + '/media/sda1 ext4       defaults,errors=remount-ro' \
                 + ',noatime	0	2">>/etc/fstab ' )
            if retc != 0:
                self.logger.error("error: unable to add " 
                                + "/dev/sda1 to /etc/fstab")
                sys.exit(255)


if __name__ == '__main__':
    """ start all tests 
    """
    dev = DeviceTestCase.get_device(nand_boot=False)
    testdev = TestDevice(dev)
    testdev.update_device()
    testdev.mount()
    unittest.main()
    
