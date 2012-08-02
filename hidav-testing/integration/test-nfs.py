#!/usr/bin/python
#
# HidaV automated test framework NFS services tests
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

import unittest, tempfile, subprocess
import device


class TestNFS( unittest.TestCase ):

    def test_exports( self ):
        expected =  "Export list for 172.29.28.100:\n"
        expected += "/           *\n"
        expected += "/media/sda1 *\n"
        exports = subprocess.check_output( ["showmount", "-e", self._remote] )
        self.assertEqual( exports, expected )


    def setUp( self ):
        self._dev = device.Device( devtype="hidav" )
        self._remote = self._dev.conn.host

        my_exports = tempfile.TemporaryFile()
        my_exports.write("/media/sda1 *(rw,fsid=0)\n")
        my_exports.write("/ *(ro)\n")

        self._dev.conn.cmd("cp /etc/exports /etc/exports.orig")
        self._dev.conn._ssh.put(my_exports, "/etc/exports")
        self._dev.conn.cmd("exportfs -ra")

    def tearDown( self ):
        self._dev.conn.cmd("mv /etc/exports.orig /etc/exports")
        pass

if __name__ == '__main__':
    unittest.main()

