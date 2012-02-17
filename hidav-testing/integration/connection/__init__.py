
import serial, binascii, logging, re

class better_serial ( serial.Serial ):
    def read_until ( self, target, trigger_write="\n" ):
        buf=""
        self.write("\n")
        while not target in buf:
            ret = self.read()
            if ret == "":
                self.write( trigger_write )
            else:
                buf += ret
        return buf

    def readline_until( self, target, trigger_write="\n" ):
        buf = self.read_until( target, trigger_write )
        buf += self.readline()
        return buf

    def _is_logged_in(self, user):
        self.write("\n")
        buf = self.readline()
        self.write("\n")
        buf += self.readline()
        return ( user + "@" in buf )

    def login( self, user, pw ):
        if self._is_logged_in( user ):
            return

        self.flushInput()
        self.flushOutput()

        self.read_until("login:", "exit\n")
        self.write( user + "\n" )

        self.read_until("Password:")
        self.write( pw + "\n" )
        self.read_until( user + "@" )

    def cmd( self, cmd ):
        self.flushInput( )
        self.flushOutput( )

        cmd = cmd.strip()
        self.write( r'_x="OUTPUT"; echo "===${_x} STARTS===";' + cmd + r';echo "===${_x} ENDS=== $?"' + "\n" )
        buf = self.readline_until( "===OUTPUT ENDS==="  )
        s   = buf.index("===OUTPUT STARTS==="); s   = buf.index("\n",  s)
        e   = buf.index("===OUTPUT ENDS===");   e   = buf.rindex("\n", 0, e)
        rc  = int(re.search(r"===OUTPUT ENDS=== (?P<retcode>[0-9]+)", buf[ e: ]).group("retcode"))
        ret = buf[ s:e ].strip()
        return rc, ret

    def logout( self ):
        self.write("\nexit\n")
        self.read_until("login:")


class Connection( object ):
    """ Connection to a device.
        The connection class abstracts a device connection via serial and/or IP.
        It implements device command processing in a request/response manner.
    """

    def _log_init( self ):
        self._logger = logging.getLogger( __name__  )
        self._logger.setLevel( logging.DEBUG )

        h   = logging.StreamHandler()
        h.setFormatter( logging.Formatter("%(asctime)s %(levelname)s %(filename)s::%(funcName)s(): %(message)s" ) )
        self._logger.addHandler( h )

    def __init__( self, serial_setup = ( "/dev/ttyUSB0", 115200, 8, 'N', 1, 5), ip_setup = ( None, "eth0" ), login = ( "root", "" ) ):
        self._log_init( )
        self._serial_setup( *serial_setup )
        self._login = login
        self._target_if = ip_setup[1]
        self.ip = ip_setup[0] if ip_setup[0] is not None else self.ip

    def _serial_setup( self, port, baud, byte, parity, stop, timeout_sec ):
        try:
            self._serial.close()
        except: pass
        self._logger.debug("(re)opening serial port %s" % port )
        self._serial = better_serial()
        self._serial.port     = port
        self._serial.baudrate = baud
        self._serial.bytesize = byte
        self._serial.parity   = parity
        self._serial.stopbits = stop
        self._serial.timeout  = timeout_sec     # 5 sec timeout
        self._serial.open()
        self._serial_state = "open"
        self._logger.debug("%s is now open." % port )
 
    def _serial_cmd( self, cmd ):
        self._logger.debug(" Executing command [%s] using serial port %s" % (cmd, self._serial.port) )
        self._serial.login( self._login[0], self._login[1] )
        rc, retstring = self._serial.cmd( cmd )
        self._logger.debug(" Command [%s] on %s returned with #%d:\n[%s]" % (cmd, self._serial.port, rc, retstring) )
        return rc, retstring

    def ip():
        def fget( self ):
            try: return self._ip
            except AttributeError:
                self._ip = self._serial_cmd( r"ifconfig | grep -A 1 '" + self._target_if + r" ' | grep 'inet addr:' | sed -e 's/.*inet\ addr:\([0-9.]\+\)\ .*/\1/'" )[1]
                return self._ip
        def fset( self, value ):
            self._ip = value
        def fdel( self ):
            try: del self._ip
            except AttributeError: pass
        return locals()
    ip = property( **ip() )




if __name__ == '__main__':
    c = Connection( ip_setup = ( None, "eth1" ), login= ("root", "") )
    print c._serial_cmd( "ls /" )[1]
    c._serial.logout()
