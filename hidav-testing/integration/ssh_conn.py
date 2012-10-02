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

import sys
import paramiko
import os
import tempfile
import logger

class SshConn(object):
    """ The SSH connection class abstracts an SSH connection to a device. 
        It implements remote command execution as well as uploading/downloading 
        of files, and also can display the contents of a remote file.
    """
    
    def __init__(self, host, username, password, session=None):
        #cmd.Cmd.__init__(self)
        """ Create a new instance of the SSH connection class. 

            :param logger:  log instance to be used in this class
            :param host:    host name or IP address of the remote host
            :param username:   username for ssh-connection
            :param password:   password for ssh-connection
            :param session:   only use for test-mocking 
        """
        self._host = host
        self._username = username
        self._password = password
        self.session = session if session is not None else paramiko.SSHClient()

    def __del__(self):
        self._host = None
        self._username = None
        self._password = None
        del self.session

    @property
    def session(self):
        """ ssh-session
        """
        return self._ssh

    @session.setter
    def session(self, val):
        self._ssh = val
        self._ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self._ssh.connect(self._host, username=self._username, password=self._password)
		#ssh.load_host_keys(os.path.expanduser(os.path.join("~", ".ssh", "known_hosts")))

    @session.deleter
    def session(self):
        self._ssh.close()
        self._ssh = None

    def cmd(self, cmd):
        """ Run a command on the remote device.

            :param cmd: the command to run
            :return:    tuple of (retcode, command_output) 
        """
        return self.session.exec_command(cmd)

