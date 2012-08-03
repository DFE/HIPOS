#!/usr/bin/python
#
# HidaV automated short samba test
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Eik Binschek <binschek@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#


import smbc
import os
import sys
import time
import device

import unittest

class TestSamba(unittest.TestCase):

	def test_complex(self):
                print "Autodetect ServerIP"
                dev = device.Device( devtype = "hidav" )
                print "Waiting for Networking to come up..."
		max_wait=120;
                while not dev.conn.has_networking():
                        time.sleep(1)
	                print (maxwait)
			maxwait -= 1;
			if maxwait == 0:
				break
		
                server=dev.conn.host
                print( "Server: {0}".format(server) )
		
		self._PID    = os.getpid()

                self._server = server
                self._share  = "pub"

                self._smb_string = "smb://{0}/{1}/{2}test.txt".format(self._server,self._share,self._PID)

		print( "Testfile: {0}".format(self._smb_string))
		print( "writing..." )
		ctx = smbc.Context()
		file_write = ctx.open(self._smb_string, os.O_CREAT | os.O_WRONLY)
		file_write.write( self._smb_string )
		file_write.close()

		print( "reading..." )
		file = ctx.open( self._smb_string )
		read_data = file.read()
		file.close()

		print( "testing..." )
		self.assertEquals(self._smb_string,read_data)

def main():
	if len(sys.argv) == 1:
		unittest.main()
	else:
		print("usage: {0}".format(sys.argv[0]))
		sys.exit(2)
	sys.exit(0)

if __name__ == '__main__':
	main()


