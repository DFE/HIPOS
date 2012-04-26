#!/usr/bin/python
#
# HidaV automated test framework serial connection handling
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

""" Helper function switching a device on/off using a serial GPIOBox. """

import serial


def power( onoff, port = "/dev/ttyUSB1" ):
    """ Power /un-power a device using a DResearch GPIOBox on a serial port.
        This helper function will issue a pin command to a GPIOBox
        connected to a serial port. Pin #1 will switch on or off depending
        on the onoff parameter.

        @param onoff: whether to switch power on or off.
        @kwparam port: serial port to use.
	"""

    ser = serial.Serial ()
    ser.port     = port
    ser.baudrate = 9600
    ser.bytesize = 8
    ser.parity   = 'N'
    ser.stopbits = 1
    ser.timeout  = 1
    cmd = "C1" if onoff else "O1"
    ser.open()
    ser.write(cmd + "\n")
    ser.close()
    

if __name__ == '__main__':
    import sys
    dev   = "/dev/ttyUSB1" if len(sys.argv) < 3 else sys.argv[2]
    onoff = True if len(sys.argv) < 2 \
		else False if sys.argv[1] in ("False", "0", "off", "OFF", "Off") \
		else True
    power( onoff, dev )
