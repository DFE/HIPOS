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
import device, logger

class TestNFS( unittest.TestCase ):
    def test_exports( self ):
        expected =  "Export list for %s:\n" % self._remote
        expected += "/           *\n"
        expected += "/media/sda1 *\n"
        exports = subprocess.check_output( ["showmount", "-e", self._remote] )
        self.assertEqual( exports, expected )

    def test_services( self ):
        # Doku
        # http://nfsv4.bullopensource.org/doc/nfs_ipv6.php
        # http://docs.fedoraproject.org/en-US/Fedora/14/html/Storage_Administration_Guide/ch-nfs.html#s2-nfs-how-daemons
        
        services = []
        # all services, protocols, and versions to be tested

        # 'name' is only used for convenience and can be changed any time
        # prognum = RPC Program Number (http://www.iana.org/assignments/rpc-program-numbers/rpc-program-numbers.xml)
        # current selection is simply based on status quo observed at coding time...
        services.append( dict(name='portmapper', prognum='100000', protocols=['tcp','udp'], versions=['2','3','4']) )
        services.append( dict(name='nfs', prognum='100003', protocols=['tcp','udp'], versions=['2','3','4']) )
        services.append( dict(name='mountd', prognum='100005', protocols=['tcp','udp'], versions=['1','2','3']) )
        services.append( dict(name='rquotad', prognum='100011', protocols=['tcp','udp'], versions=['1','2']) )
        services.append( dict(name='nlockmgr', prognum='100021', protocols=['tcp','udp'], versions=['1','3','4']) )
        services.append( dict(name='status', prognum='100024', protocols=['tcp','udp'], versions=['1']) )        
               
        for s in services:
            for v in s['versions']:
                for p in s['protocols']:
                    expected = 'program {} version {} ready and waiting\n'.format(s['prognum'], v)
                    # call: rpcinfo -T <p> <ip> <prognum> <v>
                    self._logger.debug( ['rpcinfo', '-T{}'.format(p), self._remote, s['prognum'], v] )
                    result = subprocess.check_output( ['rpcinfo', '-T{}'.format(p), self._remote, s['prognum'], v] )
                    self.assertEqual( result, expected )

    def setUp( self ):
        self._dev = device.Device( devtype="hidav" )
        self._remote = self._dev.conn.host
        self._logger = logger.init()

        my_exports = tempfile.TemporaryFile()
        my_exports.write("/media/sda1 *(rw,fsid=0)\n")
        my_exports.write("/ *(ro)\n")

        self._dev.conn.cmd("cp /etc/exports /etc/exports.orig")
        self._dev.conn._ssh.put(my_exports, "/etc/exports")
        self._dev.conn.cmd("exportfs -ra")

    def tearDown( self ):
        self._dev.conn.cmd("mv /etc/exports.orig /etc/exports")
        del self._logger

if __name__ == '__main__':
    unittest.main()

