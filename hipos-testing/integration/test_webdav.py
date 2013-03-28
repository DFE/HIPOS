#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
#
# Automated test framework WebDAV test.
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Erik Bernoth <bernoth@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

""" This module implements integration tests for the device's 
    WebDAV functions.
"""

from monk import devicetestcase
import unittest

from webdav.WebdavClient import ResourceStorer

webdav_path = "webdav" #also needs to be adapted for target usage
test_file = "davtest.txt"
test_content = "testtext"

class TestWebDAV(devicetestcase.DeviceTestCase):
    """ Class to test WebDav. """
    
    def __init__(self, *args, **kwargs):
        """ The class instantiation.
        """
        super(TestWebDAV, self).__init__("hidav", *args, **kwargs)
    
    def test_complex(self):
        """ Test WebDAV.
        
            This test writes a file via WebDAV. This file is then read again
            (via WebDav) to compare the contents. It is deleted afterwards.
        """
        #prepare
        self.logger.info("Autodetect ServerIP")
        dev = self.dev
        
        server = dev.conn.host
        self.logger.info("Server: {0}".format(server))
        host = server
        url = "davs://{0}/{1}/{2}".format(
               host, webdav_path, test_file, test_content)
        self.logger.info(url)
        expected = test_content+'\n'
        #execute
        r = ResourceStorer(url)
        r.uploadContent(expected)
        result = r.downloadContent().read()
        #assert
        self.assertEquals(expected,result)
        #cleanup
        r.delete()


    def setUp(self):
        """ Set up WebDAV test. """
        
        self.dev.wait_for_network()
   
   
if __name__ == '__main__':
    unittest.main()
