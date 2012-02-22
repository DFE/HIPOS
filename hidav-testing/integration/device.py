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

import connection

class Device( object ):
    """ This class abstracts access to a device, i.e. one single 
        physical system. """

    device_types = { 
        "hidav" : { 
            "network_setup" : ( None, "eth0" ),
            "login"         : ( "root", "" )  },
        "hipox" : {
            "network_setup" : ( None, "eth1" ),
            "login"         : ( "root", "" )  }
    }

    def __init__( self, devtype = "HidaV" ):
        try:
            self._setup = Device.device_types[ devtype.lower() ]
        except KeyError:
            raise Exception("Unknown device type %s." % devtype)

        self._conn = connection.Connection( network_setup = self._setup["network_setup"], login = self._setup["login"] )

    def firmware_version( ):
        def fget( self ):
            try:
                return self._fw_version
            except AttributeError:
                self._fw_version = self._conn.cmd( "lsb_release -r | awk '{ print $2 }'" )[1].strip()
                return self._fw_version
        def fdel( self ):
            try:                   del self._fw_version
            except AttributeError: pass
        return locals()
    firmware_version = property( **firmware_version() )






if __name__ == '__main__':
    import sys
    d = Device( devtype = "Hipox" )
    print d.firmware_version

