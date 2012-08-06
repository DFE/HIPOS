#!/usr/bin/python
#
# HidaV automated test framework board controller handling
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

"""The Board Controller class.

   This class represents the board controller of a connected device.
   The BCC is supposed to be connected to a host PC via its external serrial 
   port / FTDI cable. 
"""

import subprocess, re, threading, time, atexit

class Bcc( object ):

    def __init__( self, port = "/dev/ttyUSB0", speed = 57600, drbcc = "drbcc" ):
        """ Create a new Bcc instance.
            @param port: serial port to use for communication
            @param speed: serial port speed
            @param drbcc: Path to drbcc binary

            @raise: OSError if the drbcc binary was not found.
        """
        self.__drbcc   = drbcc
        self.__port    = port
        self.__port_br = speed

        self.poweron()

    def __del__( self ):
        """ Restore function to disable the debug mode set in __init__.
            """
        # stop faking ignition, set heartbeat to insanely high value
        self.cmd( "debugset 16,00" )

    def cmd( self, cmd="gets" ):
        """ Execute a BCC command.
            @kwparam cmd: command to be executed.

            @return: (exit_code, text_result): exit code and textual result of 
                                               command execution as returned by 
                                               drbcc
            @raise: OSError if the drbcc binary was not found.
        """
        text=""; rc = 0
        try:
            text = subprocess.check_output( 
                [   self.__drbcc, 
                    "--dev=%s,%d" % ( self.__port,
                                      self.__port_br ),
                    "--cmd=%s"    % ( cmd, ) ],
                stderr = subprocess.STDOUT)
        except subprocess.CalledProcessError:
            rc = -1

        return rc, text

    def reset( self ):
        self.poweroff()
        time.sleep(1)
        self.poweron()

    def poweron( self ):
        self.cmd( "debugset 16,010001" )
        self.heartbeat = 65535

    def poweroff( self ):
        self.cmd( "debugset 16,010000" )
        self.heartbeat = 0

    @property
    def status( self ):
        """ Bcc status information property.
            @return: 4 lines of text containing detailed bcc status, or the
                        empty string if the request failed.
        """
        return self.cmd( "gets" )[1]

    def __status( self, what ):
        """ Helper method to extract several status values
            @param what: status property, e.g. "HDD-Power" or "Ignition"
            @return: string value representing the field's status
        """
        s = re.search( what + r': (?P<stat>\w+)',
                       self.status,
                    ).group('stat')
        return s

    @property
    def ignition( self ):
        """ BCC ignition state property.
            @return: True or False
        """
        i  = self.__status("Ignition")
        return (i == "on")


    @property
    def hddpower( self ):
        """ BCC harddisk power state property.
            @return: True or False
        """
        h  = self.__status("HDD-Power")
        return (h == "on")

    @hddpower.setter
    def hddpower( self, power ):
        """ BCC harddisk power state property setter.
            @param power: True or False, i.e. whether to power the HDD or not.
        """
        ps = "1" if power else "0"
        self.cmd( "hdpower " + ps )

    @property
    def heartbeat( self ):
        """ BCC heartbeat state property.
            @return: tuple of ( curr, max ) representing the maximum count
            upon which a RESET will be triggered as well as the current count 
            (in seconds).
        """
        text = self.cmd( "debugget 1" )[1]
        values = re.search( r'data: (?P<v1>[0-9A-F]+) (?P<v2>[0-9A-F]+) '
                                + r'(?P<m1>[0-9A-F]+) (?P<m2>[0-9A-F]+)',
                            text )

        current = int( values.group("v1") + values.group("v2"), 16)
        maximum = int( values.group("m1") + values.group("m2"), 16)

        return current, maximum

    @heartbeat.setter
    def heartbeat( self, seconds ):
        """ BCC heartbeat property setter
            @param seconds: number of seconds before RESET
        """
        self.cmd( "heartbeat %s" % seconds )


if __name__ == '__main__':

    import sys
    b = Bcc()

    if len(sys.argv) > 1:
        for cmd in sys.argv[1:]:
            if cmd == "reset":
                b.reset()
                continue
            if cmd == "off":
                b.poweroff()
                continue
            if cmd == "on":
                b.poweron()
                continue
            ret, txt =  b.cmd( cmd )
            print "RETURN VALUE %s" %ret
            print txt
        sys.exit()

    while True:
        banner = "~*~AN~*~" if b.ignition   \
             else "___AUS___"

        subprocess.call("clear")
        subprocess.call( ["banner", banner] )

        print b.status
        c, m = b.heartbeat
        print "Heartbeat timeout: %s seconds of %s total" % (c, m)
        time.sleep(1)
