#!/usr/bin/python
#
# HidaV automated test framework high-level device functions
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

""" Package for the device class """

import connection, logger, power

class Device( object ):
    """ This class abstracts access to a device, i.e. one single 
        physical system. """

    DEVICE_TYPES = { 
        "hidav" : { 
            "network_setup" : ( None, "eth0" ),
            "login"         : ( "root", "" ),
            "boot_prompt"   : "HidaV boot on",
            "hw_reset"      : True,
            "serial_skip_pw": True },
        "hipox" : {
            "network_setup" : ( None, "eth1" ),
            "login"         : ( "root", "" ),
            "boot_prompt"   : "$ ",
            "hw_reset"      : False,
            "serial_skip_pw": False },
    }

    def __init__( self, devtype = "HidaV" ):
        """ Initialise a device instance.
            @param devtype: Device type, either "hidav" or "hipox" """
        try:
            self._setup = Device.DEVICE_TYPES[ devtype.lower() ]
        except KeyError:
            raise Exception("Unknown device type %s." % devtype)

        self.conn = connection.Connection( 
            network_setup = self._setup["network_setup"], 
            login = self._setup["login"],
            serial_skip_pw = self._setup["serial_skip_pw"] )
        self._logger = logger.init()
        if self._setup["hw_reset"]:
            power.power(1)

    @property
    def firmware_version( self ):
        """ Firmware version property. This is the device's
            lsb_version revision string."""
        try:
            return self._fw_version
        except AttributeError:
            self._fw_version = self.conn.cmd(  # pylint: disable-msg=W0201
                    "lsb_release -r | awk '{ print $2 }'" )[1].strip()
            return self._fw_version
    @firmware_version.deleter     # pylint: disable-msg=E1101
    def firmware_version( self ): # pylint: disable-msg=E0102
        """ Firmware version property deleter. """
        del self._fw_version

    def update_package_index( self ):
        """ Update the device's package index remotely.
            @raise: Exception if the update failed."""
        self._logger.debug("Updating the package index...")
        ret, msgs = self.conn.cmd("opkg update")
        if ret != 0:
            raise Exception("Updating the package index failed with #%s:\n%s" 
                    % (ret, msgs))

    def install_package( self, package_name ):
        """ Install a package at the device. The package index will be updated
            prior to the installation.
            @param package_name: name of the package to be installed.
            @raise: Exception if the install failed."""
        self._logger.info("Installing package %s." % package_name)
        self.update_package_index()
        ret, msgs = self.conn.cmd("opkg install %s" % package_name)
        if ret != 0:
            raise Exception("Installing package %s failed with #%s:\n%s" 
                    % (package_name, ret, msgs))

    def remove_package( self, package_name ):
        """ Remove a package from the device.
            @param package_name: name of the package to be removed.
            @raise: Exception if the install failed."""
        self._logger.info("Removing package %s." % package_name)
        ret, msgs = self.conn.cmd(
                "opkg remove --force-removal-of-dependent-packages %s" 
                % package_name)
        if ret != 0:
            raise Exception("Installing package %s failed with #%s:\n%s" 
                    % (package_name, ret, msgs))

if __name__ == '__main__':
    def standalone():
        """ Standalone function; only defined if the class is run by itself. 
            This function uses some basic capabilities of the class. It is
            thought to be used for interactive testing during development,
            and for a showcase on how to use the class. """
        import sys, time
        dev = Device( devtype = "hidav" )
        print dev.firmware_version

        print "Waiting for Networking to come up..."
        while not dev.conn.has_networking():
            time.sleep(1)
            sys.stdout.write(".")
        print "\nWe now have networking."

        print "Diffstat is not there:"
        retc, msg = dev.conn.cmd( "diffstat --help" )
        print "Retcode: %s" % retc
        print msg
        print "----------------------"

        dev.install_package( "diffstat" )
        print "Now diffstat is  there:"
        retc, msg = dev.conn.cmd( "diffstat --help" )
        print "Retcode: %s" % retc
        print msg
        print "----------------------"

        dev.remove_package ( "diffstat" )
        print "Now diffstat is gone:"
        retc, msg = dev.conn.cmd( "diffstat --help" )
        print "Retcode: %s" % retc
        print msg
        print "----------------------"
    standalone()

