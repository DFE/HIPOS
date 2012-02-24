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

import connection, logging

class Device( object ):
    """ This class abstracts access to a device, i.e. one single 
        physical system. """

    device_types = { 
        "hidav" : { 
            "network_setup" : ( None, "eth0" ),
            "login"         : ( "root", "" ),
            "boot_prompt"   : "HidaV boot on",
            "serial_skip_pw": True },
        "hipox" : {
            "network_setup" : ( None, "eth1" ),
            "login"         : ( "root", "" ),
            "boot_prompt"   : "$ ",
            "serial_skip_pw": False },
    }

    def _log_init( self ):
        self._logger = logging.getLogger( __name__  )
        self._logger.setLevel( logging.DEBUG )

        h   = logging.StreamHandler()
        h.setFormatter( logging.Formatter("%(asctime)s %(levelname)s %(filename)s::%(funcName)s(): %(message)s" ) )
        self._logger.addHandler( h )

    def __init__( self, devtype = "HidaV" ):
        try:
            self._setup = Device.device_types[ devtype.lower() ]
        except KeyError:
            raise Exception("Unknown device type %s." % devtype)

        self.conn = connection.Connection( 
            network_setup = self._setup["network_setup"], login = self._setup["login"],
            serial_skip_pw = self._setup["serial_skip_pw"],  )
        self._log_init()

    def firmware_version( ):
        def fget( self ):
            try:
                return self._fw_version
            except AttributeError:
                self._fw_version = self.conn.cmd( "lsb_release -r | awk '{ print $2 }'" )[1].strip()
                return self._fw_version
        def fdel( self ):
            try:                   del self._fw_version
            except AttributeError: pass
        return locals()
    firmware_version = property( **firmware_version() )

    def update_package_index( self ):
        self._logger.debug("Updating the package index...")
        ret, msgs = self.conn.cmd("opkg update")
        if ret != 0:
            raise Exception("Updating the package index failed with #%s:\n%s" % (ret, msgs))

    def install_package( self, package_name ):
        self._logger.info("Installing package %s." % package_name)
        self.update_package_index()
        ret, msgs = self.conn.cmd("opkg install %s" % package_name)
        if ret != 0:
            raise Exception("Installing package %s failed with #%s:\n%s" % (ret, msgs))

    def remove_package( self, package_name ):
        self._logger.info("Removing package %s." % package_name)
        ret, msgs = self.conn.cmd("opkg remove --force-removal-of-dependent-packages %s" % package_name)
        if ret != 0:
            raise Exception("Installing package %s failed with #%s:\n%s" % (ret, msgs))

if __name__ == '__main__':
    import sys, time
    d = Device( devtype = "HidaV" )
    print d.firmware_version

    print "Waiting for Networking to come up..."
    while not d.conn.has_networking():
        time.sleep(1)
        sys.stdout.write(".")
    print "\nWe now have networking."

    print "Diffstat is not there:"
    rc, msg = d.conn.cmd( "diffstat --help" )
    print "Retcode: %s" % rc
    print msg
    print "----------------------"

    d.install_package( "diffstat" )
    print "Now diffstat is  there:"
    rc, msg = d.conn.cmd( "diffstat --help" )
    print "Retcode: %s" % rc
    print msg
    print "----------------------"

    d.remove_package ( "diffstat" )
    print "Now diffstat is gone:"
    rc, msg = d.conn.cmd( "diffstat --help" )
    print "Retcode: %s" % rc
    print msg
    print "----------------------"

