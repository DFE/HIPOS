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

if len(sys.argv) != 2:
	print("usage: {0} <server_ip>".format(sys.argv[0]))
	sys.exit(2)

print( "Server: {0}".format(sys.argv[1]) )

PID    = os.getpid()

server = sys.argv[1]
share  = "pub"

smb_string = "smb://{0}/{1}/{2}test.txt".format(server,share,PID)

print( "Testfile: {0}".format(smb_string))
print( "writing..." )
ctx = smbc.Context()
file_write = ctx.open(smb_string, os.O_CREAT | os.O_WRONLY)
file_write.write( smb_string )
file_write.close()

print( "reading..." )
file = ctx.open ( smb_string )
read_data = file.read()
file.close()

print( "testing..." )
if  smb_string == read_data:
	print( " ok " )
	sys.exit(0)
else:
	print( " failed " )
	sys.exit(1)


