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

class samba_test( object ):

	def __init__(self,server_ip):

		self._PID    = os.getpid()

		self._server = server_ip
		self._share  = "pub"

		self._smb_string = "smb://{0}/{1}/{2}test.txt".format(self._server,self._share,self._PID)

	def test(self):
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
		if  self._smb_string == read_data:
			print( " ok " )
			sys.exit(0)
		else:
			print( " failed " )
			sys.exit(1)

if __name__ == '__main__':
	def standalone():
		if len(sys.argv) == 2:
			if (sys.argv[1] == "h") or (sys.argv[1] == "help") or (sys.argv[1] == "-h") or (sys.argv[1] == "-?"):
				print("usage: {0} [<server_ip>]".format(sys.argv[0]))
				sys.exit(2)
			else:
		                print( "Server: {0}".format(sys.argv[1]) )
				s=samba_test(sys.argv[1])
				s.test()
		elif len(sys.argv) == 1:
			print "Autodetect ServerIP"
	                dev = device.Device( devtype = "hidav" )
	                print "Waiting for Networking to come up..."
        	        while not dev.conn.has_networking():
                	    time.sleep(1)
	                    sys.stdout.write(".")
			server=dev.conn.host
	        	print( "Server: {0}".format(server) )
			s=samba_test(server)
			s.test()
		else:
			print("usage: {0} [<server_ip>]".format(sys.argv[0]))
			sys.exit(2)
		sys.exit(0)
	standalone()


