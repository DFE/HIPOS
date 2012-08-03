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

import unittest
from webdav.WebdavClient import ResourceStorer

webdav_path = "webdav" #also needs to be adapted for target usage
test_file = "davtest.txt"
test_content = "testtext"

class TestWebDAV(unittest.TestCase):
    def test_complex(self):
        #prepare
        print "Autodetect ServerIP"
        dev = device.Device( devtype = "hidav" )
        print "Waiting for Networking to come up..."
        max_wait=120
        while not dev.conn.has_networking():
            time.sleep(1)
            print ("wait {0}s".format(max_wait))
            max_wait -= 1
            if max_wait == 0:
                break
        server=dev.conn.host
        print( "Server: {0}".format(server) )
        host=server
        url = "davs://{0}/{1}/{2}".format(
                host,webdav_path,test_file,test_content)
        print url
        expected = test_content+'\n'
        #execute
        r = ResourceStorer(url)
        r.uploadContent(expected)
        result = r.downloadContent().read()
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
