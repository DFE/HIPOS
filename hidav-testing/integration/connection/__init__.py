
import logging, serial_conn

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

    def __init__( self, serial_setup = ( "/dev/ttyUSB0", 115200, 8, 'N', 1, 1), ip_setup = ( None, "eth0" ), login = ( "root", "" ) ):
        self._log_init( )
        self._login = login
        self._target_if = ip_setup[1]
        self._serial_setup( *serial_setup )
        if ip_setup[0]:
            self.ip = ip_setup[0]

    def _serial_setup( self, port, baud, byte, parity, stop, timeout_sec ):
        try:
            self._serial.close()
        except: pass
        self._logger.debug("(re)opening serial port %s" % port )
        self._serial = serial_conn.serial_conn( self._logger, login = self._login )
        self._serial.port     = port
        self._serial.baudrate = baud
        self._serial.bytesize = byte
        self._serial.parity   = parity
        self._serial.stopbits = stop
        self._serial.timeout  = timeout_sec
        self._serial.open()
        self._logger.debug("%s is now open." % port )
 
    def ip():
        def fget( self ):
            try: return self._ip
            except AttributeError:
                self._ip = self._serial.cmd( r"ifconfig | grep -A 1 '" + self._target_if + r" ' | grep 'inet addr:' | sed -e 's/.*inet\ addr:\([0-9.]\+\)\ .*/\1/'" )[1]
                return self._ip
        def fset( self, value ):
            self._ip = value
        def fdel( self ):
            try: del self._ip
            except AttributeError: pass
        return locals()
    ip = property( **ip() )

if __name__ == '__main__':
    import sys
    if len(sys.argv) < 3:
        print "Usage: %s <username> <password> [<ip address>]" % sys.argv[0]
        sys.exit()
    c = Connection( ip_setup = ( None, "eth1" ), login= (sys.argv[1], sys.argv[2]) )
    print c.ip
    del c.ip
    print c.ip
