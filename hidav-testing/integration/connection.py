#!/usr/bin/python
#
# HidaV automated test framework abstract connection 
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#


import logging, serial_conn, ssh_conn

class Connection( object ):
    """ Connection to a device.
        The connection class abstracts a device connection via serial and/or IP.
        It implements device command processing in a request/response manner.
    """

    def _log_init( self ):
        self._logger = logging.getLogger( __name__  )
        self._logger.setLevel( logging.DEBUG )

        h   = logging.StreamHandler()
        h.setFormatter( logging.Formatter("%(asctime)s %(levelname)s %(filename)s::%(funcName)s(): %(message)s" ) )
        self._logger.addHandler( h )


    def __init__( self, serial_setup = ( "/dev/ttyUSB0", 115200, 8, 'N', 1, 1), 
                  network_setup = ( None, "eth0" ), login = ( "root", "" ) ):
        self._log_init( )
        self._login = login
        self._target_if = network_setup[1]
        self._serial_setup( *serial_setup )
        if network_setup[0]:
            self.ip = network_setup[0]


    def _ssh( ):
        def fget( self ):
            try:    return self.__ssh
            except AttributeError:
                self.__ssh = ssh_conn.Ssh_conn( self._logger, self.ip, self._login )
                return self.__ssh
        def fset( self ):
            try:                   del self.__ssh
            except AttributeError: pass
        def fdel( self ):
            try:                   del self.__ssh
            except AttributeError: pass
        return locals()
    _ssh = property( **_ssh() )

    def _serial_setup( self, port, baud, byte, parity, stop, timeout_sec ):
        try:
            self._serial.close()
        except: pass
        self._logger.debug("(re)opening serial port %s" % port )
        self._serial = serial_conn.Serial_conn( self._logger, login = self._login )
        self._serial.port     = port
        self._serial.baudrate = baud
        self._serial.bytesize = byte
        self._serial.parity   = parity
        self._serial.stopbits = stop
        self._serial.timeout  = timeout_sec
        self._serial.open()
        self._logger.debug("%s is now open." % port )
 

    def ip():
        def fget( self ):
            try: 
                return self._ip
            except AttributeError:
                self._ip = self._serial.cmd( r"ifconfig | grep -A 1 '" + self._target_if + r" ' | grep 'inet addr:' | sed -e 's/.*inet\ addr:\([0-9.]\+\)\ .*/\1/'" )[1]
                return self._ip
        def fset( self, value ):
            self._ip = value
        def fdel( self ):
            try:                   del self._ip
            except AttributeError: pass
        return locals()
    ip = property( **ip() )


    def cmd( self, cmd ):
        self._logger.info( "Ecexuting remote command [%s]" % cmd )
        try: 
            return self._ssh.cmd( cmd )
        except:
            self._logger.exception( "Command execution of [%s] via SSH failed" % cmd)
            self._logger.info( "Falling back to serial for cmd [%s]" % cmd )
            return self._serial.cmd( cmd )
        

if __name__ == '__main__':
    import sys
    if len(sys.argv) < 3:
        print "Usage: %s <username> <password> [<ip address>]" % sys.argv[0]
        sys.exit()
    c = Connection( network_setup = ( None, "eth1" ), login= (sys.argv[1], sys.argv[2]) )
    c.cmd("ls /")
