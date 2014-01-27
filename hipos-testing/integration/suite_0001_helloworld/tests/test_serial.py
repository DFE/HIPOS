# -*- coding: utf-8 -*-
#
# Automated test framework HTTP test.
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Erik Bernoth <bernoth@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

import monk_tf.serial_io as mts
import nose.tools as nt

def test_serial_simple():
    """ send a newline and retrieve anything
    """
    # set up
    sut = mts.SerialIO("/dev/ttyUSB3", 115200, timeout=2)
    # execute
    sut.write("\n")
    out = sut.readall()
    # assert
    nt.ok_(out)
