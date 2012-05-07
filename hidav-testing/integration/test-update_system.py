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

import sys, os, time, threading, logger, power

from device import Device

def wait_networking( dev ):
    print "  Waiting for Networking to come up..."
    while not dev.conn.has_networking():
        time.sleep(1)
    print "\n  We now have networking."

def powercycle( ):
    print "Powercycling the device"
    power.power(0)
    time.sleep(1)
    power.power(1)

def init( ):
    """ Initialise the device handle, boot to NAND flash.
        @returns: a device handle, i.e. a class Device instance.
    """

    print "  Initialising the device."
    print "  (Device will be powercycled in 60 secs if unresponsive)"
    power.power(1)
    t = threading.Timer( 60, powercycle )
    t.start()
    dev = Device( devtype = "hidav" )
    wait_networking( dev )    
    t.cancel()

    # set currently used partitions to the first partitions, 
    # so upgrade (detecting this) will use the other partitions
    dev.bootconfig = { "kernel" : 2, "rootfs" : 4 } 

    print "  Firmware version: %s" % dev.firmware_version
    print "  Boot config (current):"
    print "     Kernel:      /dev/mtd%s" % dev.bootconfig["kernel"]
    print "     rootfs: /dev/romblock%s" % dev.bootconfig["rootfs"]
    print "     epoch :             #%s" % dev.bootconfig["epoch"]

    print "  Booting into NAND..."
    dev.reboot( to_nand = True )
    wait_networking( dev )    
    return dev


def prepare_upgrade( dev ):
    """ Configure the system to upgrade the second partitions (via bootconfig)
        and make sure that the second partitions contain garbage so the
        upgrade will actually happen. 
        @param dev: handle of a device to work with.
    """

    print "  Preparing the upgrade / invalidating second partitions."

    dev.conn.cmd("dd if=/dev/zero of=/tmp/z bs=2048 count=1")
    dev.conn.cmd("nandwrite -p /dev/mtd3 /tmp/z")
    dev.conn.cmd("nandwrite -p /dev/mtd5 /tmp/z")


def run_upgrade( dev ):
    """ Run a system upgrade, i.e. install
        kernel and rootfs via the package management.
        @param dev: handle of a device to work with.
    """
    print "  Installing kernel package..."
    dev.install_package("kernel-image-2.6.37", force=1)

    print "  Installing hydraip-image-pkg package..."
    dev.install_package("hydraip-image-pkg", force=1)

    print "  Re-reading boot configuration..."
    del dev.bootconfig
    print "  Boot config (new):"
    print "     Kernel:      /dev/mtd%s" % dev.bootconfig["kernel"]
    print "     rootfs: /dev/blockdev%s" % dev.bootconfig["rootfs"]
    print "     epoch :             #%s" % dev.bootconfig["epoch"]


def run_diag( dev ):
    print "  Kernel uname: %s" % dev.conn.cmd("uname -a")
    
    print "  Kernel package info:"
    for line in dev.conn.cmd("opkg info kernel-image-2.6.37"):
        print "\t\t%s" % line
    print "  rootfs package info:"
    for line in dev.conn.cmd("opkg info hydraip-image-pkg"):
        print "\t\t%s" % line


def die_of_timeout( dev ):
    print "\nTIMEOUT while waiting for device to come up."
    print "Aborting test."
    print "Refer to the log file %s for details." % logger.filename
    print "Switching off the device."
    power.power(0)
    print "Terminating test."
    os._exit(1)


def test():
    print "\nInitialising the device."
    dev = init()
    bc_old = dev.bootconfig

    print "\nPreparing the upgrade."
    prepare_upgrade( dev )

    print "\nUpgrading the system."
    run_upgrade( dev )

    # force re-read
    del dev.bootconfig
    bc = dev.bootconfig

    print "\nSetting reboot timeout to 120 seconds."
    t = threading.Timer( 120, die_of_timeout, args=[ dev , ] )
    t.start()
    
    # TODO: add a timeout
    print "Rebooting into newly installed system..."
    dev.reboot( to_nand = True )
    t.cancel()

    wait_networking( dev )    

    print "\nRunning diagnostics."
    run.diag( dev )
    print "\nTest run successful."
    sys.exit(0)

test()
