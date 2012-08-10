#!/usr/bin/python2
# -*- coding: utf-8 -*-
#
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Erik Bernoth <bernoth@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

import os
import sys
import time
import device
import httplib
import unittest

import devicetestcase

test_html = "<html><body><h1>It works!</h1></body></html>" #default from lighty

class TestHTTP(devicetestcase.DeviceTestCase):
    def test_complex(self):
        #prepare
        server = self.dev.conn.host
        print( "Server: {0}".format(server) )
        url=server
        expected = test_html
        connection = httplib.HTTPConnection(url)
        #execute
        connection.request('GET','')
        result = connection.getresponse().read()
        #assert
        self.assertEquals(expected,result)
        #cleanup
        connection.close()

    def setUp(self):
        self.wait_for_network()

def main():
    if len(sys.argv) == 1:
        unittest.main()
    else:
        print("usage: {0}".format(sys.argv[0]))
        sys.exit(2)
    sys.exit(0)
    
if __name__ == '__main__':
    main()

