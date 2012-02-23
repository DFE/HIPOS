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

import serial, re, urllib, time 

class Serial_conn( serial.Serial ):
    """Serial connection to a device.
       The serial class implements device access via the serial port.
    """
    def __init__( self, logger, login = ( "root", ""), 
                  skip_pass = True, boot_prompt="HidaV boot on", 
                  needs_hw_reset = True, *args, **kwargs ):
        self._logger = logger
        serial.Serial.__init__( self, *args, **kwargs )
        self._login = login
        self._skip_pass = skip_pass
        self._boot_prompt = boot_prompt
        self._needs_hw_reset = needs_hw_reset

    def read_until ( self, target, trigger_write="\n", timeout=None ):
        self._logger.debug( "reading 'til [%s], triggering output with [%s]" 
                % (target, urllib.quote( trigger_write )) )
        buf=""; log_line=""
        if timeout:
            old_to = self._timeout
            self._timeout = timeout
        self.write("\n")
        while not target in buf:
            ret = self.read()
            if ret == "":
                self._logger.debug( "Triggering with [%s]" % urllib.quote( trigger_write ) )
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
        buf = self.read_until( target, trigger_write )
        buf += self.readline()
        return buf

    def _is_logged_in(self):
        self.write("\n")
        buf = self.readline()
        self.write("\n")
        buf += self.readline()
        return ( self._login[0] + "@" in buf )

    def _is_in_bootloader(self):
        self.write("\n")
        buf = self.readline()
        self.write("\n")
        buf += self.readline()
        return ( self._boot_prompt in buf )

    def login( self ):
        if self._is_logged_in( ):
            self._logger.debug( "User %s already logged in." % self._login[0] )
            return

        if self._is_in_bootloader( ):
            self._logger.debug( "System has stopped at bootloader; booting..." )
            # the bootloader loses bytes on the serial line, so we send this a few times
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
            self._logger.debug( "Sending password for user %s." % self._login[0] )
            self.read_until("Password:")
            self.write( self._login[1] + "\n" )

        self.read_until( self._login[0] + "@" )
        # make sure no kernel messages go to the serial console while
        # we are remote-controlling the device
        self.write( 'echo "0 0 0 0" > /proc/sys/kernel/printk\n')

    def cmd( self, cmd ):
        self.flushInput( )
        self.flushOutput( )

        cmd = cmd.strip()
        self._logger.info( "Executing command [%s]" % cmd )

        self.login()

        self.write( r'_x="OUTPUT"; echo "===${_x} STARTS===";' + cmd + r';echo "===${_x} ENDS=== $?"' + "\n" )
        buf = self.readline_until( "===OUTPUT ENDS==="  )
        s   = buf.index("===OUTPUT STARTS==="); s   = buf.index("\n",  s)
        e   = buf.index("===OUTPUT ENDS===");   e   = buf.rindex("\n", 0, e)
        rc  = int(re.search(r"===OUTPUT ENDS=== (?P<retcode>[0-9]+)", buf[ e: ]).group("retcode"))
        ret = buf[ s:e ].strip()
        self._logger.info( "Command [%s] ret: #%d" % (cmd, rc) )
        self._logger.debug( "Command [%s] output:\n[%s]" % (cmd, ret) )
        return rc, ret

    def logout( self ):
        if self._is_logged_in():
            self._logger.debug( "Logging out." )
            self.write("\nexit\n")
            self.read_until("login:")
    
    def reboot( self, reboot_cmd="halt", sync=False, stop_at_bootloader=False ):
        self._logger.info( "Rebooting %s" % ("and stopping at bootloader" if
                stop_at_bootloader else "synchronously" if sync else ".") )
        self.login()
        self.write( reboot_cmd + "\n" )
        buf=""
        if self._needs_hw_reset:
            import reset
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
    logging.basicConfig( level = logging.DEBUG, format="%(asctime)s %(levelname)s %(filename)s::%(funcName)s(): %(message)s" )
    if len(sys.argv) != 3:
        print "Usage: %s <username> <password>" % sys.argv[0]
        sys.exit()
    l = logging.getLogger( __name__ + "-TEST" )
    sc = Serial_conn( l, (sys.argv[1], sys.argv[2] ))
    sc.port     = "/dev/ttyUSB0"
    sc.baudrate = 115200
    sc.bytesize = 8
    sc.parity   = 'N'
    sc.stopbits = 1
    sc.timeout  = 1
    sc.open()
    print sc.cmd( "ls /" )[1]
    print " ######## "
    print " ######## Rebooting..."
    print " ######## "
    boot_log = sc.reboot( sync = True )
    print " ######## "
    print " ######## Reboot OK"
    print " ######## \n"
    print "   --- reboot messages ---  \n"
    print boot_log
    print "   --- ------ -------- ---  \n"
    print " ######## "
    print " ######## Rebooting into bootloader..."
    print " ######## "
    boot_log = sc.reboot( stop_at_bootloader = True )
    print " ######## Device stopped in boot loader"
    print "   --- reboot messages ---  \n"
    print boot_log
    print "   --- ------ -------- ---  \n"


