#!/usr/bin/python2
# -*- coding: utf-8 -*-
#
# NightOwl - who tests your unit tests?
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Erik Bernoth <bernoth@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

import unittest
from webdav.WebdavClient import ResourceStorer

host = "localhost" #later switch to target IP/domain
webdav_path = "webdav" #also needs to be adapted for target usage
test_file = "davtest.txt" #again probably rename it
test_content = 'dav\n'

class TestWebDAV(unittest.TestCase):

    def test_read(self):
        url = "davs://{0}/{1}/{2}".format(
                host,webdav_path,test_file,test_content)
        expected = test_content
        r = ResourceStorer(url)
        result = r.downloadContent().read()
        self.assertEquals(expected,result)

    def test_complex(self):
        url = "davs://{0}/{1}/2{2}".format(
                host,webdav_path,test_file,test_content)
        expected = 'vad\n'
        r = ResourceStorer(url)
        r.uploadContent(expected)
        result = r.downloadContent().read()
        r.delete()
        self.assertEquals(expected,result)

if __name__ == '__main__':
    unittest.main()
