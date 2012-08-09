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

""" This module implements integration tests for the device's 
    NFS server functions.
"""
import unittest
import tempfile
import subprocess
import time

import device
import logger

                                  # TestCase has too many public methods.
class TestNFS(unittest.TestCase): # pylint: disable-msg=R0904
    """ The NFS tests class. There's onlyone for now. """

    def test_exports(self):
        """ Test exported NFS shares.
            This test checks whether the NFS shares exported in setUp
            are listed. The command line tool 'showmounts -e' is used for
            this.
        """
        expected =  "Export list for %s:\n" % self._remote
        expected += "/                         *\n"
        expected += "/media/sda1/nfs-forbidden 1.1.1.2\n"
        expected += "/media/sda1/nfs-allowed   %s\n" % self._remote
        exports = subprocess.check_output(["showmount", "-e", self._remote])
        self.assertEqual(exports, expected)


    def __test_service(self, proto, prog_num, version):
        """ Knock a single service's door, check results """
        expected = 'program {} version {} ready and waiting\n'.format(
                      prog_num, version)

        # call: rpcinfo -T <p> <ip> <prognum> <v>
        cmd = ['rpcinfo', '-T{}'.format(proto), 
                self._remote, 
                prog_num, 
                version]

        self._logger.debug("Running %s" % cmd)

        result = subprocess.check_output(cmd)
        self.assertEqual(result, expected)


    def test_services(self):
        """ Test NFS RPC services.
            This test knocks on every service/port/protocol combination
            we have observed being available at implementation time.
            The test calls the "rpcinfo" command line tool for this.

            NFS Documentation:
            - http://nfsv4.bullopensource.org/doc/nfs_ipv6.php
            - http://docs.fedoraproject.org/en-US/Fedora/14/html/Storage_Administration_Guide/ch-nfs.html#s2-nfs-how-daemons
            - http://www.iana.org/assignments/rpc-program-numbers/rpc-program-numbers.xml
        """
        
        services = []

        # all services, protocols, and versions to be tested
        # 'name' is only used for convenience and can be changed any time
        # prognum = RPC Program Number 
        # Current selection is based on status quo observed at coding time.
        
        services.append(dict(name='portmapper', prognum='100000', 
                             protocols=['tcp','udp'], versions=['2','3','4']))
        services.append(dict(name='nfs', prognum='100003', 
                             protocols=['tcp','udp'], versions=['2','3','4']))
        services.append(dict(name='mountd', prognum='100005', 
                             protocols=['tcp','udp'], versions=['1','2','3']))
        services.append(dict(name='rquotad', prognum='100011', 
                             protocols=['tcp','udp'], versions=['1','2']))
        services.append(dict(name='nlockmgr', prognum='100021', 
                             protocols=['tcp','udp'], versions=['1','3','4']))
        services.append(dict(name='status', prognum='100024', 
                             protocols=['tcp','udp'], versions=['1']))        
        for service in services:
            for version in service['versions']:
                for proto in service['protocols']:
                    self.__test_service(proto, service['prognum'], version)


    def __cmd(self, cmd, expected=0):
        """ Helper to run a command and check its return value """
        ret, output = self._dev.conn.cmd(cmd)

        if expected is not None:
            self.assertEqual( ret, expected, msg=
                "Command [%s] failed; ret #%s:\n%s" 
                % ( cmd, ret, output ) )


    def test_mount_cpy(self) :
        """ Mount a NFS share, copy a file, and check it's contents. """
        self.__cmd("mount -t nfs "
                    +"%s:/media/sda1/nfs-allowed " % self._remote
                    +"/media/sda1/nfs-mount")

        self.__cmd("dd if=/dev/urandom bs=102400 "
                    +"count=100 "
                    +"of=/media/sda1/nfs-mount/blob")

        self.__cmd("cd /media/sda1/nfs-mount/ "
                    +"&& md5sum blob > blob.md5")

        self.__cmd("cd /media/sda1/nfs-allowed/ "
                    +"&& md5sum -c blob.md5")

        self.__cmd("umount /media/sda1/nfs-mount")


    def test_invalid_mount(self) :
        """ Try to mount a NFS share we're not allowed to mount. """
        self.__cmd("mount -t nfs "
                    +"%s:/media/sda1/nfs-illegal " % self._remote
                    +"/media/sda1/nfs-mount",
                   expected=32)


    def setUp(self): # I didn't come up with this! pylint: disable-msg=C0103
        """ Test setup routine: prepare the NFS exports used by the tests. """
        self._dev = device.Device( devtype="hidav" )
        print "Waiting for Networking to come up..."
        max_wait = 120
        while not self._dev.conn.has_networking():
            time.sleep(1)
            print ("wait {0}s".format(max_wait))
            max_wait -= 1
            if max_wait == 0:
                break

        self._remote = self._dev.conn.host
        self._logger = logger.init()

        self.__cmd( "mkdir -p /media/sda1/nfs-allowed "
                            +"/media/sda1/nfs-forbidden "
                            +"/media/sda1/nfs-mount" )

        self.__cmd( "chmod 777 /media/sda1/nfs-allowed "
                             +"/media/sda1/nfs-forbidden "
                             +"/media/sda1/nfs-mount" )

        my_exports = tempfile.TemporaryFile()
        my_exports.write("/media/sda1/nfs-allowed   %s(rw,fsid=0)\n" 
                                                        % self._remote)
        my_exports.write("/media/sda1/nfs-forbidden 1.1.1.2(rw)\n")
        my_exports.write("/ *(ro)\n")

        self._dev.conn.cmd("cp /etc/exports /etc/exports.orig")
        self._dev.conn._ssh.put(my_exports, "/etc/exports")
        self._dev.conn.cmd("exportfs -ra")


    def tearDown( self ): # Wrong Naming (CamelCase) pylint: disable-msg=C0103
        """ After-Test restore routine: restore device's original exports. """
        self._dev.conn.cmd("rm -f /media/sda1/nfs-mount/blob")
        self._dev.conn.cmd("rm -f /media/sda1/nfs-mount/blob.md5")
        self._dev.conn.cmd("mv /etc/exports.orig /etc/exports")
        del self._logger

if __name__ == '__main__':
    unittest.main()

