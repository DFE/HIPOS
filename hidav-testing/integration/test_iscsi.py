#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
#
# HidaV automated test framework SAMBA test (short).
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Eik Binschek <binschek@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

""" This module implements integration tests for the device's 
    iSCSI functions.
"""

from Gordon import devicetestcase
import unittest

class TestIscsi(devicetestcase.DeviceTestCase):
    """ Class to test iSCSI. """

    def __init__(self, *args, **kwargs):
        """ The class instantiation.
        """
        super(TestIscsi, self).__init__("hidav", *args, **kwargs)
    
    def test_complex(self):
        """ Test a iSCSI share.
        
            This test writes and reads a iSCSI share.
        """
        
        self.logger.info("Autodetect ServerIP")
        dev = self.dev

        server = dev.conn.host
        self.logger.info("Server: {0}".format(server))


        self.logger.info("Setup iSCSI-Target: start")
        retc, msg = dev.conn.cmd("ls /media/sda1")
        if retc != 0:
            retc, msg = dev.conn.cmd("mkdir /media/sda1")
        retc, msg = dev.conn.cmd("mount | grep /media/sda1")
        if retc != 0:
            retc, msg = dev.conn.cmd("mount /dev/sda1 /media/sda1")

        retc, msg = dev.conn.cmd("systemctl stop iscsi-target.service")

        retc, msg = dev.conn.cmd("chmod 777 /media/sda1")
        retc, msg = dev.conn.cmd("ls /media/sda1/iscsi.target")
        if retc != 0:
            retc, msg = dev.conn.cmd("dd if=/dev/zero of=/media/sda1/" +
                                    "iscsi.target bs=1024k count=100")
        retc, msg = dev.conn.cmd("losetup -a | grep /media/sda1/iscsi.target")
        if retc != 0:
            retc, msg = dev.conn.cmd("losetup /dev/loop0 /media/sda1/" +
                                    "iscsi.target")
        retc, msg = dev.conn.cmd('grep "Target iqn.2030-01.de.dresearch-fe'+
                                ':iSCSI.sda.target1" /etc/ietd.conf')
        if retc != 0:
            retc, msg = dev.conn.cmd('echo "Target iqn.2030-01.de.dresearch'+
                                    '-fe:iSCSI.sda.target1" >> '+
                                    '/etc/ietd.conf')
            retc, msg = dev.conn.cmd('echo "         Lun 0 Path=/dev/loop0'+
                                    ',Type=blockio" >> /etc/ietd.conf')
        retc, msg = dev.conn.cmd("systemctl start iscsi-target.service")
        self.assertEquals(retc, 0)
        self.logger.info("Setup iSCSI-Target: finished")
        
        self.logger.info("Setup open-iscsi initiator: start")
        self.logger.error("Setup open-iscsi initiator: missing initiator")
        self.logger.error("Setup open-iscsi initiator: missing initiator")
        self.logger.error("""
Packet install:
sudo aptitude install open-iscsi open-iscsi-utils smartmontools

Target query:
binschek@CD-400-548:~$ sudo iscsiadm -m discovery -t st -p 172.29.28.160
172.29.28.160:3260,1 iqn.2030-01.de.dresearch-fe:iSCSI.sda.target1
binschek@CD-400-548:~$ 

Target config check:
binschek@CD-400-548:~$ sudo iscsiadm -m node
172.29.28.160:3260,1 iqn.2030-01.de.dresearch-fe:iSCSI.sda.target1
binschek@CD-400-548:~$ 

Target login:
binschek@CD-400-548:~$ sudo iscsiadm -m node --targetname """+
""""iqn.2030-01.de.dresearch-fe:iSCSI.sda.target1" --portal "172.29.28."""+
"""160:3260" --login
Logging in to [iface: default, target: iqn.2030-01.de.dresearch-fe:iSCSI"""+
""".sda.target1, portal: 172.29.28.160,3260]
Login to [iface: default, target: iqn.2030-01.de.dresearch-fe:iSCSI.sda."""+
"""target1, portal: 172.29.28.160,3260]: successful
binschek@CD-400-548:~$ 

iSCSI disk check:
binschek@CD-400-548:~$ sudo smartctl -a /dev/sdh
smartctl 5.41 2011-06-09 r3365 [x86_64-linux-3.2.0-30-generic] (local build)
Copyright (C) 2002-11 by Bruce Allen, http://smartmontools.sourceforge.net

Vendor:               IET     
Product:              VIRTUAL-DISK    
Revision:             0   
User Capacity:        104.857.600 bytes [104 MB]
Logical block size:   512 bytes
Serial number:        4b5fb8451b985fa7ec19f24ae0fba8ac
Device type:          disk
Local Time is:        Tue Aug 28 14:11:19 2012 CEST
Device does not support SMART

Error Counter logging not supported

[GLTSD (Global Logging Target Save Disable) set. Enable Save with '-S on']
Device does not support Self Test logging
binschek@CD-400-548:~$ 

Partition create:
binschek@CD-400-548:~$ sudo parted /dev/sdh
(parted) mklabel gpt
(parted) mkpart primary 0% 100%                                         
Warning: The resulting partition is not properly aligned for best performance.
Ignore/Cancel? Ignore                                                     
(parted) print                                                            
Model: IET VIRTUAL-DISK (scsi)
Disk /dev/sdh: 105MB
Sector size (logical/physical): 512B/512B
Partition Table: gpt

Number  Start   End    Size   File system  Name     Flags
 1      17,4kB  105MB  105MB               primary

(parted)                                               
(parted) quit

Filesystem create:
binschek@CD-400-548:~$ sudo mkfs.ext4 -L iSCSI /dev/sdh1
mke2fs 1.42 (29-Nov-2011)
Filesystem label=iSCSI
OS type: Linux
Block size=1024 (log=0)
Fragment size=1024 (log=0)
Stride=0 blocks, Stripe width=0 blocks
25688 inodes, 102364 blocks
5118 blocks (5.00%) reserved for the super user
First data block=1
Maximum filesystem blocks=67371008
13 block groups
8192 blocks per group, 8192 fragments per group
1976 inodes per group
Superblock backups stored on blocks: 
	8193, 24577, 40961, 57345, 73729

Allocating group tables: done                            
Writing inode tables: done                            
Creating journal (4096 blocks): done
Writing superblocks and filesystem accounting information: done 

binschek@CD-400-548:~$ 

Test:
binschek@CD-400-548:~$ mkdir sdh
binschek@CD-400-548:~$ sudo mount /dev/sdh1 sdh
""")


        self.assertEquals("open-iscsi initiator is missing", False)


    def setUp(self):
        """ Set up SAMBA test. """
        self.dev.wait_for_network()


if __name__ == '__main__':
    unittest.main()
