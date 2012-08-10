#!/usr/bin/python
#
# HidaV automated nas test
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Eik Binschek <binschek@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

""" Tool to start all available test on a HidaV-device """

import unittest

from test_samba import TestSamba
from test_webdav import TestWebDAV
# from test_nfs import TestNFS
# from test_http import TestHTTP

if __name__ == '__main__':
    """ start all tests """
    unittest.main()
    