#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
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

""" Package for the connection class """

import logger, serial_conn, ssh_conn, re

class Connection(object):
    """ Connection to a device.

        The connection class abstracts a device connection via serial and/or IP.
        It implements device command processing in a request/response manner.
    """

    def __init__(self, serial_setup = ( "/dev/ttyUSB1", 115200, 8, 'N', 1, 1), 
                 network_setup = ( None, "eth0" ), login = ( "root", "" ),
                 boot_prompt = "HidaV boot on", serial_skip_pw = True,
                 reset_cb = None):
        """ Initialize a new connection instance.

            :param serial_setup: set of init options for the serial backend:
                ("port", baudrate, data bits, parity, stop bits, timeout_sec)
            :param network_setup: set of init options for network connections:
                ("ip or hostname", "network dev ON THE DEVICE") 
            :param login: tuple of (username, pass) for the device
            :param boot_prompt: string for identifying the boot loader prompt
            :param serial_skip_pw: True if the serial login does not prompt
                for a password
            :param reset_cb: callback to run when resetting the device
        """
        self._logger = logger.init()
        self._login = login
        self._target_if = network_setup[1]
        self._serial = self._serial_setup(*serial_setup, 
                            skip_pass = serial_skip_pw, 
                            boot_prompt = boot_prompt,
                            reset_cb = reset_cb)
        if network_setup[0]:
            self.host = network_setup[0]


    @property
    def _ssh(self):
        """ SSH connection property. Will be created on the fly as required."""
        try:    
            return self.__ssh
        except AttributeError:
            self.__ssh = ssh_conn.SshConn( # pylint: disable-msg=W0201
                    self._logger, self.host, self._login ) 
        return self.__ssh
    
    
    @_ssh.deleter # new-style properties unknown to pylint: disable-msg=E1101
    def _ssh(self): # pylint: disable-msg=E0102
        """ SSH connection property deleter. Deletes the property. """
        del self.__ssh

    
    def _serial_setup(self, port, baud, byte, parity, stop, timeout_sec, 
                      skip_pass, boot_prompt, reset_cb):
        """ Set up the serial communication back-end. """
        self._logger.debug("(re)opening serial port %s" % port)
        ser = serial_conn.SerialConn(self._logger, 
                        login = self._login, skip_pass = skip_pass, 
                        boot_prompt = boot_prompt,
                        reset_cb = reset_cb)
        ser.port     = port
        ser.baudrate = baud
        ser.bytesize = byte
        ser.parity   = parity
        ser.stopbits = stop
        ser.timeout  = timeout_sec
        ser.open()
        self._logger.debug("%s is now open." % port)
        return ser
 

    @property
    def host(self): # pylint: disable-msg=E0202
        """ The Host property. This can be set via init using network_setup,
            or it might be determined automatically at run-time by reading the
            IP address from the device using the serial connection.
        """
        try: 
            return self._host
        except AttributeError:
            ret = self._serial.cmd( 
                    r"ifconfig | grep -A 1 '" + self._target_if + r"'"
                  + r"| grep 'inet addr:' "
                  + r"| sed -e 's/.*inet\ addr:\([0-9.]\+\)\ .*/\1/'")[1]
            try:
                self._host = re.search( 
                    r'(?P<ip>[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3})'
                        , ret).group("ip")
            except AttributeError:
                raise Exception(
                    "Unable to collect device's IP address via serial port %s"
                    % self._serial.port)
        return self._host


    @host.setter # new-style properties unknown to pylint: disable-msg=E1101
    def host(self, value): # pylint: disable-msg=E0102,E0202
        """ Set the Host property. """
        self._host = value # pylint: disable-msg=W0201


    @host.deleter # new-style properties unknown to pylint: disable-msg=E1101
    def host(self): # pylint: disable-msg=E0102,E0202
        """ Delete the Host property. """
        try:
            del self._host
        except AttributeError:
            pass


    def cmd(self, cmd):
        """ Run a remote command on the device. 

            The command will be issued via SSH if the host name or IP address is 
            known to the connection (i.e. C{has_networking()} returns True),
            and via serial line otherwise.

            :param cmd:  command to be issued
            :return:     tuple of (retcode, command_output) 
        """
        self._logger.info("Ecexuting remote command [%s]" % cmd)
        try: 
            return self._ssh.cmd(cmd)
        except:
            self._logger.exception("Command execution of [%s] via SSH failed"
                    % cmd)
            self._logger.info("Falling back to serial for cmd [%s]" % cmd)
            return self._serial.cmd(cmd)


    def has_networking(self):
        """ (Actively) Check whether we currently have networking, i.e.
            whether the address of the remote device is known.
            This will trigger extraction of the IP address via serial
            if the address is currently unknown.
        """
        del self.host
        try:    
            self.host = self.host
        except: 
            return False
        return True
    
    
def main():
    """ Standalone function; only defined if the class is run by itself. 
        This function uses some basic capabilities of the class. It is
        intended to be used for interactive testing during development,
        and as a showcase on how to use the class. 
    """
    import sys, time
    if len(sys.argv) < 3:
        print "Usage: %s <username> <password> [<ip address>]" % sys.argv[0]
        sys.exit()

    import bcc
    b = bcc.Bcc()

    conn = Connection( network_setup = (None, "eth1"), 
                        login = (sys.argv[1], sys.argv[2]) )

    print "Waiting for Networking to come up..."
    while not conn.has_networking():
        time.sleep(1)
        sys.stdout.write(".")
    print "\nWe now have networking."

    print "Executing ls /"
    print conn.cmd("ls /")[1]

    print "Now shutting down the device."
    b.heartbeat = 30
    conn._serial._reset_cb = None
    conn._serial.reboot()


if __name__ == '__main__':
    main()
    