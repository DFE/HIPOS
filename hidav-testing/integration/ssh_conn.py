#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
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

import socket
import sys
import libssh2
import os
import tempfile

import logger

class SshConn(object):
    """ The SSH connection class abstracts an SSH connection to a device. 
        It implements remote command execution as well as uploading/downloading 
        of files, and also can display the contents of a remote file.
    """
    
    def __init__(self, logger, host, login = ("root", "")):
        """ Create a new instance of the SSH connection class. 

            :param logger:  log instance to be used in this class
            :param host:    host name or IP address of the remote host
            :param login:   tuple of (username, pass)
        """
        self._logger = logger
        self._host = host
        self._login = login


    def _session(self):
        """ Session property. This is the current SSH session. It will be 
            created when needed.

            :return: the session (which will be created if it isn't there yet)
        """
        self._logger.info("Opening session for user %s to %s" 
                           % (self._login[0], self._host))
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect( (self._host, 22) )
        sock.setblocking(1)

        self.__session = libssh2.Session() # pylint: disable-msg=W0201
        self.__session.startup(sock)
        self.__session.userauth_password(self._login[0], self._login[1])
        return self.__session


    def cmd(self, cmd):
        """ Run a command on the remote device.

            :param cmd: the command to run
            :return:    tuple of (retcode, command_output) 
        """
        self._logger.info("Executing command [%s]" % cmd)
        s = self._session()
        chan = s.open_session()
        # fixup: explicitly set PATH since we missed /usr/sbin
        # in the default env at some point.
        ret = chan.execute("export PATH=\"$PATH:/bin:/sbin:/usr/bin"
                           + ":/usr/sbin:/usr/local/bin:/usr/local/sbin\";"
                           +  cmd + " 2>&1")
        if ret == 0:
            buf = ""
            while True:
                ret = chan.read(1024)
                if ret == "" or ret is None:
                    break
                buf += ret
            chan.close()
            rcd = chan.exit_status()
            self._logger.info("Command [%s] ret: #%d" % (cmd, rcd))
            self._logger.debug("Command [%s] output:\n[%s]" % (cmd, buf))
            return rcd, buf
        else:
            raise Exception(
                "Remote execution of %s failed; ret %s, session error %s." 
                    % (cmd, ret, s.last_error()) )


    def get(self, local_file, remote_path):
        """ Get a file from the remote system.

            :param local_file:  file-like object for storing the file
            :param remote_path: path to the file (including file name) 
                                on the remote system
            :return:            None 
        """
        self._logger.info("Receiving file [%s] from device" % (remote_path))
        start_offs = local_file.tell()
        chan = self._session().scp_recv(remote_path)
        local_file.write(chan.read())
        chan.close()
        size = local_file.tell() - start_offs
        self._logger.info("Received [%s], %s bytes, from device" 
                                % (remote_path, size))


    def cat(self, remote_path):
        """ Return a buffer containing the contents of a remote file.

            :param remote_path: path to the file (including file name) 
                                on the remote system
            :return:            buffer containing the file's contents 
        """
        tfl = tempfile.TemporaryFile()
        self.get(tfl, remote_path)
        tfl.seek(0)
        return tfl.read()


    def put(self, local_file, remote_path, remote_mode=0644):
        """ Send a file to the remote system.

            :param local_file:  file-like object to be sent
            :param remote_path: path to the file (including file name) 
                                on the remote system
            :return:            None 
        """
        local_file.seek(0, os.SEEK_END)
        size = local_file.tell()
        local_file.seek(0)

        self._logger.info("Sending file [%s], mode %s, %d bytes, to device" 
                            % (remote_path, remote_mode, size))
        chan = self._session().scp_send(remote_path, remote_mode, size)
        chan.write(local_file.read())
        chan.close()
        self._logger.info("File [%s] transmission successful" % remote_path)  


def main():
    """ Standalone function; only defined if the class is run by itself. 
        This function uses some basic capabilities of the class. It is
        intended to be used for interactive testing during development,
        and as a showcase on how to use the class. 
    """
    if len(sys.argv) != 4:
        print "Usage: %s <host> <username> <password>" % sys.argv[0]
        sys.exit()
    ssh = SshConn(logger.init(), sys.argv[1], (sys.argv[2], sys.argv[3]))
    print ssh.cmd("echo $PATH")[1]
    print ssh.cmd("ls /")[1]
    print ssh.cat("/etc/resolv.conf")
    print ssh.cmd("dd if=/dev/zero of=/tmp/z bs=1024 count=1")[1]
    print ssh.cmd("depmod")[1]
    print ssh.cmd("nandwrite")[1]
    print ssh.cmd("flash_erase")[1]
    tfl = tempfile.TemporaryFile()
    tfl.write("This is an example file transmission.")
    ssh.put(tfl, "/home/root/test")
    print ssh.cat("/home/root/test")


if __name__ == '__main__':
    main()
    
