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

import serial, re, urllib, time

class SerialConn( serial.Serial ):
    """Serial connection to a device.
       The serial class implements device access via the serial port.
    """
    def __init__( self, logger, login = ( "root", ""), 
                  skip_pass = True, boot_prompt="HidaV boot on", 
                  reset_cb = None, *args, **kwargs ):
        """Initialise a new instance of the serial communication class.
            @param logger:         Log object to be used by this class.
            @param login:          tuple of ( username, pass )
            @param skip_pass:      True if the login does not prompt for a password
            @param boot_prompt:    The prompt to identify the boot loader with
            @param reset_cb:       Custom RESET callback to run when the device
                                    is rebooted.
        """
        self._logger = logger
        try:    
            super(SerialConn, self).__init__( *args, **kwargs )
        except AttributeError: 
            pass
        self._login = login
        self._skip_pass = skip_pass
        self._boot_prompt = boot_prompt
        self._reset_cb = reset_cb

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

    def dump_receive_buffer ( self ):
        """ Return the contents of the serial input buffer """
        return self.read( self.inWaiting() )        

    def readline_until( self, target, trigger_write="\n" ):
        """ Read up to a trigger text, read the whole line, then stop.
            This method works just like C{read_until()} with the exception that 
            it will read the whole last line containing the target string."""
        buf = self.read_until( target, trigger_write )
        buf += self.readline()
        return buf

    def __boot_state( self ):
        """ Return the current system state.
            @return: string containing  "bootloader", "login", "shell", or 
                     "UNKNOWN"
        """
        buf = self.read_until( "\n", timeout=3 )

        return "shell"  if self._login[0] + "@" in buf \
                        else "login" if "login:" in buf \
                        else "bootloader" if self._boot_prompt in buf \
                        else "UNKNOWN"

    def login( self ):
        """ Login to a device.
            This method will log in to a device. It will check wheter we are 
            already logged in (in which case the method succeeds early), and 
            whether we're currently in the boot loader (in which case the 
            method will boot the device, then log in)."""

        while self.__boot_state() == "UNKNOWN":
            self._logger.debug("Waiting for a known system state...")

        if self.__boot_state() == "shell":
            self._logger.debug("Already logged in.")
            return

        if self.__boot_state( ) == "bootloader":
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

        self.read_until("login:", "exit\n", timeout=20 )
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


    def __run_cmd( self, cmd ):
        """ Generic command executor for both boot loader and linux shell cmds.
            This method will run a command, wrapping the command's output into
            boundaries for later extraction,
            @param cmd: command to execute
            @return:    string buffer containing command output.
        """
        self.flushInput( )
        self.flushOutput( )

        self.read_until( "\n", timeout=30 )
        
        self.write(   r'remote_trigger="OUTPUT";'
                    + r'echo "===${remote_trigger} STARTS===";' 
                    + cmd 
                    + r';echo "===${remote_trigger} ENDS==="' + "\n" )

        buf = self.readline_until( "===OUTPUT ENDS==="  )

        start = buf.index("===OUTPUT STARTS===")
        start = buf.index("\n", start)

        end = buf.index("===OUTPUT ENDS===")
        end = buf.rindex("\n", 0, end)

        return buf[ start : end ].strip()


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
        cmd = cmd.strip() + r'; echo -e "\nRETCODE=$?"'
        self._logger.info( "Executing command [%s]" % cmd )

        self.login()
        buf = self.__run_cmd( cmd )

        rcd = int( re.search(r'RETCODE=(?P<retcode>[0-9]+)', 
                        buf).group("retcode") )

        self._logger.info( "Command [%s] ret: #%d" % (cmd, rcd) )
        return rcd, buf

    def logout( self ):
        """ Log out of the system. """
        if self.__boot_state() == "shell":
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
        self.write( "\n" + reboot_cmd + "\n" )
        buf = ""
        if self._reset_cb:
            buf += self.readline_until( "Detaching DM devices" )
            self._reset_cb()
        if stop_at_bootloader:
            buf += self.read_until("U-Boot")
            buf += self.read_until(self._boot_prompt, ".\n", 0.001)
        elif sync:
            buf += self.readline_until("login:")
        self._logger.info("OK")
        return buf


    def boot_to_nand( self, kernel_partition = None, rootfs_partition = None,
                            sync = False ):
        """ Reboot the device to NAND.
            @kwparam kernel_partition: kernel partition number to boot into.
            @kwparam rootfs_partition: root fs parition number to boot into.
            @kwparam sync:  Wait for the reboot to happen; return only after
                                the device rebooted 
                                and a login is available. Defaults to False.
            @return:            All the console messages from when the reboot has been issued up to
                                the point the method stopped reading.
            """
        buf = ""
	if kernel_partition != None:
	        kernel_offset = "0x200000" if kernel_partition == 2 else "0xC00000"
	        self._logger.info( "Rebooting to NAND (kernel: mtd%s, rootfs: mtd%s)."
                            % (kernel_partition, rootfs_partition) )

        buf += self.reboot( sync=True, stop_at_bootloader = True )

        self.flushInput( )
        self.flushOutput( )
        self.flush( )
        time.sleep(1)

        if kernel_partition != None:
		self._logger.debug( "Setting kernel offset to %s." % kernel_offset )
	        self.write("\nsetenv kernel_offset %s\n" % kernel_offset)
	        self.flush( )
        	time.sleep(1)

	if rootfs_partition != None:
	        self._logger.debug( "Setting kernel root to /dev/blockdev%s." 
                                % rootfs_partition)
	        self.write("\nsetenv rootfs_device /dev/blockdev%s\n" 
                                % rootfs_partition )
	        self.flush( )
	        time.sleep(1)

        self._logger.debug( "Issuing 'run bootnand'." )
        self.write("\nrun bootnand\n")
        self.flush( )
        time.sleep(1)
        if sync:
            buf += self.readline_until("login:")
        self._logger.debug( "Now rebooted to NAND." )

        return buf



if __name__ == '__main__':
    import logger, sys, bcc

    b = bcc.Bcc()
    def rst():
        b.reset = 1

    def standalone():
        """ Standalone function; only defined if the class is run by itself. 
            This function uses some basic capabilities of the class. It is
            thought to be used for interactive testing during development,
            and for a showcase on how to use the class. """
        if len(sys.argv) < 3:
            print "Usage: %s <username> <password> [<command>]" % sys.argv[0]
            sys.exit()
        scn = SerialConn( logger.init(), (sys.argv[1], sys.argv[2] ),
                reset_cb=rst )
        scn.port     = "/dev/ttyUSB1"
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

        print " ######## "
        print " ######## ls /"
        print " ######## "
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
        print scn.dump_receive_buffer()

    standalone()


