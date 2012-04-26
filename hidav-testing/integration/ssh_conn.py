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

""" Package for the SSH / SCP Connection class """

import socket, sys, libssh2, os, tempfile

class SshConn( object ):
    """ The SSH connection class abstracts an SSH connection to a device. 
        It implements remote command execution as well as 
        uploading/downloading files, and also can display the contents 
        of a remote file.
        """
    def __init__( self, logger, host, login = ( "root", "" ) ):
        """ Create a new instance of the SSH connection class. 
            @param logger:  log instance to be used in this class
            @param host:    host name or IP address of the remote host
            @param login:   Tuple of ( username, pass )
        """
        self._logger = logger
        self._host = host
        self._login = login


    @property
    def _socket( self ):
        """ Socket property. This is the currently used socket of the
            SSH connection. Will be created on demand, can be closed
            by deleting the property.
            Return the socket; create it if it isn't there yet """
        try:    
            return self.__socket
        except AttributeError:
            self._logger.info("Opening tcp connection to %s" % (self._host))
            sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
            sock.connect( (self._host, 22) )
            sock.setblocking(1)
            self.__socket = sock # pylint: disable-msg=W0201
            return self.__socket
    @_socket.deleter # new-style properties unknown to pylint: disable-msg=E1101
    def _socket( self ): # pylint: disable-msg=E0102
        """ Delete the socket """
        del self.__socket


    @property
    def _session( self ):
        """ Session property. This is the current SSH session. It will be 
            created when needed.
             Return the session; create it if it isn't there yet """
        try:    
            return self.__session
        except AttributeError:
            self._logger.info("Opening session for user %s to %s" 
                                % (self._login[0], self._host))
            self.__session = libssh2.Session() # pylint: disable-msg=W0201
            self.__session.startup( self._socket )
            self.__session.userauth_password( 
                    self._login[0], self._login[1] )
            return self.__session
    @_session.deleter # new-style property unknown to pylint: disable-msg=E1101
    def _session( self ): # pylint: disable-msg=E0102
        """ Delete the session """
        del self.__session


    def cmd( self, cmd ):
        """ Run a command on the remote device.
            @param cmd: the command to run
            @return:    tuple of ( retcode, command_output ) """
        self._logger.info( "Executing command [%s]" % cmd )
        chan   = self._session.open_session()
        if 0 <= chan.execute( cmd ):
            buf = ""
            while True:
                ret = chan.read( 1024 )
                if ret == "" or ret is None:
                    break
                buf += ret
            chan.close()
            rcd = chan.exit_status()
            self._logger.info( "Command [%s] ret: #%d" % (cmd, rcd) )
            self._logger.debug( "Command [%s] output:\n[%s]" % (cmd, buf) )
            return rcd, buf
        else:
            raise Exception("Remote execution of %s failed." % cmd)


    def get( self, local_file, remote_path ):
        """ Get a file from the remote system.
            @param local_file:  File-like object for storing the file
            @param remote_path: Path to the file (including file name) 
                                on the remote system
            @return:            None """
        self._logger.info( "Receiving file [%s] from device" % (remote_path) )
        start_offs = local_file.tell()
        chan = self._session.scp_recv( remote_path )
        local_file.write( chan.read() )
        chan.close()
        size = local_file.tell() - start_offs
        self._logger.info( "Received [%s], %s bytes, from device" 
                                % (remote_path, size) )

    def cat( self, remote_path ):
        """ Return a buffer containing the contents of a remote file.
            @param remote_path: Path to the file (including file name) 
                                on the remote system
            @return:            buffer containing the file's contents """
        tfl = tempfile.TemporaryFile()
        self.get( tfl, remote_path )
        tfl.seek(0)
        return tfl.read()

    def put( self, local_file, remote_path, remote_mode=0644 ):
        """ Send a file to the remote system.
            @param local_file:  File-like object to be send
            @param remote_path: Path to the file (including file name) 
                                on the remote system
            @return:            None """
        local_file.seek( 0, os.SEEK_END )
        size = local_file.tell()
        local_file.seek( 0 )

        self._logger.info( "Sending file [%s], mode %s, %d bytes, to device" 
                            % (remote_path, remote_mode, size) )
        chan = self._session.scp_send( remote_path, remote_mode, size )
        chan.write( local_file.read() )
        chan.close()
        self._logger.info( "File [%s] transmission successful" % remote_path)  


if __name__ == '__main__':
    def standalone():
        """ Standalone function; only defined if the class is run by itself. 
            This function uses some basic capabilities of the class. It is
            thought to be used for interactive testing during development,
            and for a showcase on how to use the class. """
        import logging
        logging.basicConfig( level = logging.INFO, format=
           "%(asctime)s %(levelname)s %(filename)s::%(funcName)s():" 
         + "%(message)s" )
        if len(sys.argv) != 4:
            print "Usage: %s <host> <username> <password> <host>" % sys.argv[0]
            sys.exit()
        log = logging.getLogger( __name__ + "-TEST" )
        ssh = SshConn( log, sys.argv[1], (sys.argv[2], sys.argv[3] ) )
        print ssh.cmd("ls /")[1]
        print ssh.cat( "/etc/resolv.conf" )
        tfl = tempfile.TemporaryFile()
        tfl.write("This is an example file transmission.")
        ssh.put(tfl, "/home/root/test")
        print ssh.cat( "/home/root/test" )
    standalone()
