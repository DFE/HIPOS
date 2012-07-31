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

import subprocess, re, threading, time

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
        # run an initial command to check the settings
        self.__bcc_cmd()

        self.reset = False
        self.__wd_thread = threading.Thread( target = self.__wd )
        self.__wd_thread.daemon = True
        self.__wd_thread.start()
    
    def __bcc_cmd( self, cmd="gets" ):
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

    def __wd( self ):
        """ Bcc watchdog thread.
            This thread will read ignition and calm the watchdog & power the HDD
            if the device is switched on.
            It will power-cycle the device if self.__reset has been set,
            then un-set self.__reset.
        """
        while True:
            if self.ignition:

                if self.reset:
                    self.hddpower = 0
                    self.heartbeat = 0
                    time.sleep( 1 )
                    self.reset = False

                self.heartbeat = 2
                self.hddpower = 1
            else:
                self.hddpower = 0
            
            time.sleep( 1 )
                
    @property
    def status( self ):
        """ Bcc status information property.
            @return: 4 lines of text containing detailed bcc status, or the
                        empty string if the request failed. Example output:

            status_cb: raw data: 02 00 00 00 00 00 20 0b 0b 00 08 00 db 00 09 58 00 00 00 fe 00 a9 00 b2 00 00 00 00 00 00 00 00 00 00 00 00 00
            SPIs/SPOs: Ignition: off, HDD-Sense: open, OXE_INT1: 0, OXE_INT2: 0, XOSC_ERR: 0, GPI-Power: off, HDD-Power: off
            GPInputs 1..6: 0 0 0 0 0 0   GPOutputs 1..4: 0 0 0 0  RTC Temp : 32 deg C  Accel X, Y, Z: 42mg, 31mg, 855mg
            Voltages: 23.92V  0.00V  2.54V  1.69V  1.78V  0.00V  0.00V  0.00V  0.00V  
        """
        return self.__bcc_cmd( "gets" )[1]

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
        self.__bcc_cmd( "hdpower " + ps )

    @property
    def heartbeat( self ):
        """ BCC heartbeat state property.
            @return: tuple of ( max, curr ) representing the maximum count
            upon which a RESET will be triggered as well as the current count 
            (in seconds).
        """
        return ( 0,0 ) # not yet implemented in the board controller

    @heartbeat.setter
    def heartbeat( self, seconds ):
        """ BCC heartbeat property setter
            @param seconds: number of seconds before RESET
        """
        self.__bcc_cmd( "heartbeat %s" % seconds )


if __name__ == '__main__':
    b = Bcc()
    while True:

        banner = ">>>RST<<<" if b.reset    \
             else "~*~AN~*~" if b.ignition   \
             else "___AUS___"

        subprocess.call("clear")
        subprocess.call( ["banner", banner] )

        print b.status
        time.sleep(1)





            


