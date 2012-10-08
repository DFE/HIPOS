#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
#
# HidaV automated test framework HTTP test.
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Erik Bernoth <bernoth@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

import unittest

import libssh2

import ssh_conn

class TestSshConn(unittest.TestCase):
    """ Class to test ssh-connectivity.
    
        FIXME: Rewrite for libssh2
    """
    
    def test_ssh_connectivity(self):
        """ Test ssh establish and closing connection
        """
        #prepare
        server = '172.29.21.366'
        username = 'heins'
        password = 'hans'
        mock = MockLibsshConn(server, username, password)
        #execute
        ssh = ssh_conn.SshConn(server, username, password, mock)
        del ssh
        #assert
        self.assertTrue(mock.check_usage)

    def test_ssh_cmd(self):
        """ Test execute single ssh cmd
        """
        #prepare
        server = '172.29.21.366'
        username = 'heins'
        password = 'hans'
        cmd = 'uptime'
        expected = 'ret_val'
        mock = MockLibsshConn(server, username, password, cmd, expected)
        #execute
        ssh = ssh_conn.SshConn(server, username, password, mock)
        result = ssh.cmd(cmd)
        del ssh
        #assert
        self.assertEquals(expected, result)


class MockLibsshConn(object):
    """Class to mock paramiko-ssh-client"""

    def __init__(self, server, username, password, cmd = None, ret_val = None):


if __name__ == '__main__':
    unittest.main()
