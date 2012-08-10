#!/usr/bin/python
#
# HidaV automated short samba test
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Eik Binschek <binschek@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#


import smbc
import os
import sys
import time

import devicetestcase
import unittest


class TestSamba(devicetestcase.DeviceTestCase):
    """ test SAMBA class """

    def test_complex(self):
        """ write and read a SAMBA share """
        print "Autodetect ServerIP"
        dev = self.dev
        self.wait_for_network()

        server=dev.conn.host
        print( "Server: {0}".format(server) )

        self._PID    = os.getpid()

        self._server = server
        self._share  = "pub"

        retc, msg = dev.conn.cmd( "mount /dev/sda1 /mnt/pub" )
        retc, msg = dev.conn.cmd( "chmod 777 /mnt/pub" )

        self._smb_string = "smb://{0}/{1}/{2}test.txt".format(self._server,self._share,self._PID)

        print( "Testfile: {0}".format(self._smb_string))
        print( "writing..." )
        ctx = smbc.Context()
        file_write = ctx.open(self._smb_string, os.O_CREAT | os.O_WRONLY)
        file_write.write( self._smb_string )
        file_write.close()

        print( "reading..." )
        file = ctx.open( self._smb_string )
        read_data = file.read()
        file.close()

        retc, msg = dev.conn.cmd( "rm /mnt/pub/{0}test.txt".format(self._PID) )
        retc, msg = dev.conn.cmd( "umount /mnt/pub" )

        print( "testing..." )
        self.assertEquals(self._smb_string,read_data)


if __name__ == '__main__':
    unittest.main()
    
