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

""" Helper function which performs a RESET on a device. """

import serial


def reset( port = "/dev/ttyUSB1" ):
    """ Reset a device using a DResearch GPIOBox on a serial port.
        This helper function will issue a pin command to a GPIOBox
        connected to a serial port. Pin #1 will switch for 2 seconds,
        connecting the shutter, which should be connected to the
        device's RESET pins."""
    ser = serial.Serial ()
    ser.port     = port
    ser.baudrate = 9600
    ser.bytesize = 8
    ser.parity   = 'N'
    ser.stopbits = 1
    ser.timeout  = 1
    ser.open()
    ser.write("P114\n")
    ser.close()
    

if __name__ == '__main__':
    reset()
