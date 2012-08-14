#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
#
# HidaV automated test framework HTTP test.
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
    HTTP functions.
"""

import httplib
import unittest

import devicetestcase

test_html = "<html><body><h1>It works!</h1></body></html>" #default from lighty

class TestHTTP(devicetestcase.DeviceTestCase):
    """ Class to test HTTP. """
    
    def test_complex(self):
        """ Test HTTP.
        
            Test for known content on the server.
        """
        #prepare
        server = self.dev.conn.host
        self.logger.info("Server: {0}".format(server))
        url = server
        expected = test_html
        connection = httplib.HTTPConnection(url)
        #execute
        connection.request('GET','')
        result = connection.getresponse().read()
        #assert
        self.assertEquals(expected, result)
        #cleanup
        connection.close()


    def setUp(self):
        """ Set up HTTP test. """
        self.dev.wait_for_network()


if __name__ == '__main__':
    unittest.main()
    