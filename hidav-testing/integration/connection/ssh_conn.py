#!/usr/bin/python
#
# HidaV automated test framework ssh connection handling
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

import socket, sys, libssh2, os

class Ssh_conn( object ):
    def __init__( self, logger, host, login = ( "root", "" ) ):
        self._logger = logger
        self._host = host
        self._login = login


    def _socket( ):
        def fget( self ):
            try:    
                return self.__socket
            except AttributeError:
                self._logger.info("Opening tcp connection to %s" % (self._host))
                s = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
                s.connect( (self._host, 22) )
                s.setblocking(1)
                self.__socket = s
                return self.__socket
        def fdel( self ):
            try:    del self.__socket
            except: pass
        return locals()
    _socket = property( **_socket() )


    def _session( ):
        def fget( self ):
            try:    
                return self.__session
            except AttributeError:
                self._logger.info("Opening session for user %s to %s" % (self._login[0], self._host))
                self.__session = libssh2.Session()
                self.__session.startup( self._socket )
                self.__session.userauth_password( self._login[0], self._login[1] )
                return self.__session
        def fdel( self ):
            try:
                del self.__session
            except: pass
        return locals()
    _session = property( **_session() )


    def cmd( self, cmd ):
        self._logger.info( "Executing command [%s]" % cmd )
        c   = self._session.open_session()
        rc  = c.execute( cmd )
        buf = ""
        while True:
            ret = c.read( 1024 )
            if ret == "" or ret is None:
                break
            buf += ret
        c.close()
        self._logger.info( "Command [%s] ret: #%d" % (cmd, rc) )
        self._logger.debug( "Command [%s] output:\n[%s]" % (cmd, buf) )
        return rc, buf


    def get( self, local_file, remote_path ):
        self._logger.info( "Receiving file [%s] from device" % (remote_path) )
        start_offs = local_file.tell()
        c = self._session.scp_recv( remote_path )
        local_file.write( c.read() )
        c.close()
        size = local_file.tell() - start_offs
        self._logger.info( "Received [%s], %s bytes, from device" % (remote_path, size) )

    def cat( self, remote_path ):
        t = tempfile.TemporaryFile()
        self.get( t, remote_path )
        t.seek(0)
        return t.read()

    def put( self, local_file, remote_path, remote_mode=0644 ):
        local_file.seek( 0, os.SEEK_END )
        size = local_file.tell()
        local_file.seek( 0 )

        self._logger.info( "Sending file [%s], mode %s, %d bytes, to device" 
                            % (remote_path, remote_mode, size) )
        c = self._session.scp_send( remote_path, remote_mode, size )
        c.write( local_file.read() )
        c.close()
        self._logger.info( "File [%s] transmission successful" % remote_path)  


if __name__ == '__main__':
    import logging, sys, tempfile
    logging.basicConfig( level = logging.INFO, format="%(asctime)s %(levelname)s %(filename)s::%(funcName)s(): %(message)s" )
    if len(sys.argv) != 4:
        print "Usage: %s <host> <username> <password> <host>" % sys.argv[0]
        sys.exit()
    l = logging.getLogger( __name__ + "-TEST" )
    ssh = Ssh_conn( l, sys.argv[1], (sys.argv[2], sys.argv[3] ) )
    print ssh.cmd("ls /")[1]
    print ssh.cat( "/etc/resolv.conf" )
    t = tempfile.TemporaryFile()
    t.write("This is an example file transmission.");
    ssh.put(t, "/home/root/test")
    print ssh.cat( "/home/root/test" )
