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

import paramiko

import ssh_conn_new

class TestSshConn(unittest.TestCase):
    """ Class to test ssh-connectivity. """
    
    def test_ssh_connectivity(self):
        """ Test ssh establish and closing connection
        """
        #prepare
        server = '172.29.21.366'
        username = 'heins'
        password = 'hans'
        mock = MockSshConn(server, username, password)
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
        mock = MockSshConn(server, username, password, cmd, expected)
        #execute
        ssh = ssh_conn.SshConn(server, username, password, mock)
        result = ssh.cmd(cmd)
        del ssh
        #assert
        self.assertEquals(expected, result)


class MockSshConn(object):
    """Class to mock paramiko-ssh-client"""

    def __init__(self, server, username, password, cmd = None, ret_val = None):
        self.__check_policy = False
        self.__check_connect = False
        self.__check_close = False
        self.__def_server = server
        self.__def_username = username
        self.__def_password = password
        self.__def_cmd = cmd
        self.__def_exec_return_val = ret_val

    def set_missing_host_key_policy(self, val):
        self.__check_policy = isinstance(val, paramiko.MissingHostKeyPolicy)

    def connect(self, server, username, password):
        self.__check_connect = self.__def_server == server \
                and self.__def_username == username \
                and self.__def_password == password

    def close(self):
        self.__check_close = True

    def exec_command(self, cmd):
        return self.__def_exec_return_val

    def check_usage(self):
        return self.__check_policy and self.__check_connect and self.__check_close


if __name__ == '__main__':
    unittest.main()
