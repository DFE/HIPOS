#!/usr/bin/python
#
# HidaV automated test framework serial connection handling
#
# Copyright (C) 2012 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

""" Package for the Serial Connection class """

import serial, re, urllib, reset

class SerialConn( serial.Serial ):
    """Serial connection to a device.
       The serial class implements device access via the serial port.
    """
    def __init__( self, logger, login = ( "root", ""), 
                  skip_pass = True, boot_prompt="HidaV boot on", 
                  needs_hw_reset = True, *args, **kwargs ):
        """Initialise a new instance of the serial communication class.
            @param logger:         Log object to be used by this class.
            @param login:          tuple of ( username, pass )
            @param skip_pass:      True if the login does not prompt for a password
            @param boot_prompt:    The prompt to identify the boot loader with
            @param needs_hw_reset: True if the device requires a HW reset for a reboot.
                                   This requires additional hardware in the test setup.
        """
        self._logger = logger
        try:    
            super(SerialConn, self).__init__( *args, **kwargs )
        except AttributeError: 
            pass
        self._login = login
        self._skip_pass = skip_pass
        self._boot_prompt = boot_prompt
        self._needs_hw_reset = needs_hw_reset

    def read_until ( self, target, trigger_write="\n", timeout=None ):
        """ Read up to a trigger text, then stop.
            @param target:          Target string to match
            @param trigger_write:   A string to be sent via serial if a read timeout occurs
            @param timeout:         Custom timeout for C{trigger_write}
            @return:                All the text read up to the point where C{target} 
                                    appeared, including C{target}."""
        self._logger.debug( "reading 'til [%s], triggering output with [%s]" 
                % (target, urllib.quote( trigger_write )) )
        buf      = ""
        log_line = ""
        if timeout:
            old_to = self._timeout
            self._timeout = timeout
        self.write("\n")
        while not target in buf:
            ret = self.read()
            if ret == "":
                self._logger.debug( "Triggering with [%s]" 
                                        % urllib.quote( trigger_write ) )
                self.write( trigger_write )
            else:
                buf += ret
                if ret == '\n':
                    self._logger.debug( "[%s]"  % log_line.strip() )
                    log_line = ""
                else:
                    log_line += ret
        self._logger.debug( "Got it: [%s]" % target )
        if timeout:
            self._timeout = old_to
        return buf

    def readline_until( self, target, trigger_write="\n" ):
        """ Read up to a trigger text, read the whole line, then stop.
            This method works just like C{read_until()} with the exception that 
            it will read the whole last line containing the target string."""
        buf = self.read_until( target, trigger_write )
        buf += self.readline()
        return buf

    def _is_logged_in(self):
        """ Check whether we are currently logged in at the remote system. """
        self.write("\n")
        buf = self.readline()
        self.write("\n")
        buf += self.readline()
        return ( self._login[0] + "@" in buf )

    def _is_in_bootloader(self):
        """ Check whether the boot loader prompt is currently active 
            at the device. """
        self.write("\n")
        buf = self.readline()
        self.write("\n")
        buf += self.readline()
        return ( self._boot_prompt in buf )

    def login( self ):
        """ Login to a device.
            This method will log in to a device. It will check wheter we are 
            already logged in (in which case the method succeeds early), and 
            whether we're currently in the boot loader (in which case the 
            method will boot the device, then log in)."""
        if self._is_logged_in( ):
            self._logger.debug( "User %s already logged in." % self._login[0] )
            return

        if self._is_in_bootloader( ):
            self._logger.debug( "System has stopped at bootloader; booting..." )
            # the bootloader loses bytes on the serial line, so repeat this 
            self.write("\nboot\n")
            self.flush()
            self.write("\nboot\n")
            self.flush()
            self.write("\nboot\n")
            self.flush()

        self._logger.info( "Login for user %s." % self._login[0] )
        self.flushInput()
        self.flushOutput()

        self.read_until("login:", "exit\n")
        self.write( self._login[0] + "\n" )

        if not self._skip_pass:
            self._logger.debug( "Sending password for user %s." 
                                    % self._login[0] )
            self.read_until("Password:")
            self.write( self._login[1] + "\n" )

        self.read_until( self._login[0] + "@" )
        # make sure no kernel messages go to the serial console while
        # we are remote-controlling the device
        self.write( 'echo "0 0 0 0" > /proc/sys/kernel/printk\n')

    def cmd( self, cmd ):
        """ Execute a linux user space command on the remote system.
            This method will execute a command on the remote system,
            returning the retcode of that command and all the output
            which appeared on the serial console while the command was
            executed.

            NOTE that the command to be run MUST be guaranteed to return; 
            so issuing "halt" or "reboot" is a bad idea. Also, Linux
            user space is required for execution (e.g. when calculating
            the return value of the command executed). C{cmd()} does not
            currently work with the boot loader prompt.

            @param cmd: The command to run
            @return:    Tuple of (retcode, command output) """
        self.flushInput( )
        self.flushOutput( )

        cmd = cmd.strip()
        self._logger.info( "Executing command [%s]" % cmd )

        self.login()

        self.write(   r'_x="OUTPUT"; echo "===${_x} STARTS===";' 
                    + cmd + r';echo "===${_x} ENDS=== $?"' + "\n" )

        buf = self.readline_until( "===OUTPUT ENDS==="  )

        start = buf.index("===OUTPUT STARTS===")
        start = buf.index("\n", start)

        end = buf.index("===OUTPUT ENDS===")
        end = buf.rindex("\n", 0, end)

        ret = buf[ start : end ].strip()

        rcd = int( re.search(r"===OUTPUT ENDS=== (?P<retcode>[0-9]+)", 
                        buf[ end: ]).group("retcode") )

        self._logger.info( "Command [%s] ret: #%d" % (cmd, rcd) )
        self._logger.debug( "Command [%s] output:\n[%s]" % (cmd, ret) )
        return rcd, ret

    def logout( self ):
        """ Log out of the system. """
        if self._is_logged_in():
            self._logger.debug( "Logging out." )
            self.write("\nexit\n")
            self.read_until("login:")
    
    def reboot( self, reboot_cmd="halt", sync=False, stop_at_bootloader=False ):
        """ Reboot the system.
            This method will reboot the system, kicking it with a HW reset if the serial class was
            asked to do so at instanciation time.
            By default the method will issue a reboot command and then return immediately.
            @param reboot_cmd:  actual command to trigger the reboot. Defaults to "halt" since most of
                                our systems are self-resetting :)
            @param sync:        Wait for the reboot to happen; return only after the device rebooted 
                                and a login is available. Defaults to False.
            @param stop_at_bootloader:
                                Reboot, then wait for boot loader messages on the serial line, then
                                interrupt the boot loader. This function returns after we got a boot
                                loader prompt.
            @return:            All the console messages from when the reboot has been issued up to
                                the point the method stopped reading. May be the empty string if
                                an asynchronous reboot has been requested.
        """
        self._logger.info( "Rebooting %s" % ("and stopping at bootloader" if
                stop_at_bootloader else "synchronously" if sync else ".") )
        self.login()
        self.write( reboot_cmd + "\n" )
        buf = ""
        if self._needs_hw_reset:
            buf += self.readline_until( "Detaching DM devices" )
            reset.reset()
        if stop_at_bootloader:
            buf += self.read_until("U-Boot")
            buf += self.read_until(self._boot_prompt, ".\n", 0.001)
        elif sync:
            buf += self.readline_until("login:")
        self._logger.info("OK")
        return buf

if __name__ == '__main__':
    import logging, sys
    def standalone():
        """ Standalone function; only defined if the class is run by itself. 
            This function uses some basic capabilities of the class. It is
            thought to be used for interactive testing during development,
            and for a showcase on how to use the class. """
        logging.basicConfig( level = logging.DEBUG, 
            format = 
            "%(asctime)s %(levelname)s %(filename)s::%(funcName)s(): %(message)s" )
        if len(sys.argv) < 3:
            print "Usage: %s <username> <password> [<command>]" % sys.argv[0]
            sys.exit()
        logger = logging.getLogger( __name__ + "-TEST" )
        scn = SerialConn( logger, (sys.argv[1], sys.argv[2] ))
        scn.port     = "/dev/ttyUSB0"
        scn.baudrate = 115200
        scn.bytesize = 8
        scn.parity   = 'N'
        scn.stopbits = 1
        scn.timeout  = 1
        scn.open()

        if len(sys.argv) > 3:
            for cmd in sys.argv[3:]:
                print "\n ######## "
                print " ######## Running custom command >>>> %s <<<<" % cmd
                print " ######## \n"
                retcode, bla = scn.cmd(cmd)
                print "\n\n#######\nRETCODE: %s, EXEC MSGS:\n%s" % (retcode, bla)
            sys.exit()

        print scn.cmd( "ls /" )[1]
        print " ######## "
        print " ######## Rebooting..."
        print " ######## "
        bootlog = scn.reboot( sync = True )
        print " ######## "
        print " ######## Reboot OK"
        print " ######## \n"
        print "   --- reboot messages ---  \n"
        print bootlog
        print "   --- ------ -------- ---  \n"
        print " ######## "
        print " ######## Rebooting into bootloader..."
        print " ######## "
        bootlog = scn.reboot( stop_at_bootloader = True )
        print " ######## Device stopped in boot loader"
        print "   --- reboot messages ---  \n"
        print bootlog
        print "   --- ------ -------- ---  \n"

    standalone()


