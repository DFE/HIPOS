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

test_html = "<html><body><h1>It works!</h1></body></html>" #default from lighty

class TestHTTP(unittest.TestCase):
    def test_complex(self):
        #prepare
        print "Autodetect ServerIP"
        dev = device.Device( devtype = "hidav" )
        print "Waiting for Networking to come up..."
        while not dev.conn.has_networking():
            sys.stdout.write(".")
            sys.stdout.flush()
            time.sleep(1)
        server=dev.conn.host
        print( "Server: {0}".format(server) )
        host=server
        url = "davs://{0}".format(
                host,webdav_path,test_file,test_content)
        print url
        expected = test_html
        connection = httplib.HttpConnection(url)
        #execute
        connection.request('GET','')
        result = connection.getresponse().read()
        #assert
        self.assertEquals(expected,result)
        #cleanup
        r.delete()

def main():
    if len(sys.argv) == 1:
        unittest.main()
    else:
        print("usage: {0}".format(sys.argv[0]))
        sys.exit(2)
    sys.exit(0)
    
if __name__ == '__main__':
    main()

