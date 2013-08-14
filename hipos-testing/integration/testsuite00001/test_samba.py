#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
#
# Automated test framework SAMBA test (short).
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
    SAMBA functions.
"""

import smbc
import os

from monk import devicetestcase
import unittest

class TestSamba(devicetestcase.DeviceTestCase):
    """ Class to test SAMBA. """

    def __init__(self, *args, **kwargs):
        """ The class instantiation.
        """
        super(TestSamba, self).__init__("hidav", *args, **kwargs)
    
    def test_complex(self):
        """ Test a SAMBA share.
        
            This test writes and reads a SAMBA share.
        """
        
        self.logger.info("Autodetect ServerIP")
        dev = self.dev

        server = dev.conn.host
        self.logger.info("Server: {0}".format(server))

        self._PID    = os.getpid()
        self._server = server
        self._share  = "pub"

        retc, msg = dev.conn.cmd("mount /dev/sda1 /mnt/pub")
        retc, msg = dev.conn.cmd("chmod 777 /mnt/pub")

        self._smb_string = "smb://{0}/{1}/{2}test.txt".format(self._server,
                                                              self._share,
                                                              self._PID)

        self.logger.info("Testfile: {0}".format(self._smb_string))
        self.logger.info("writing...")
        ctx = smbc.Context()
        file_write = ctx.open(self._smb_string, os.O_CREAT | os.O_WRONLY)
        file_write.write(self._smb_string)
        file_write.close()

        self.logger.info("reading...")
        file = ctx.open(self._smb_string)
        read_data = file.read()
        file.close()

        retc, msg = dev.conn.cmd("rm /mnt/pub/{0}test.txt".format(self._PID))
        retc, msg = dev.conn.cmd("umount /mnt/pub")

        self.logger.info("testing...")
        self.assertEquals(self._smb_string,read_data)


    def setUp(self):
        """ Set up SAMBA test. """
        self.dev.wait_for_network()


if __name__ == '__main__':
    unittest.main()
    
