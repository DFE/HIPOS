
from connection import Connection

c = Connection( ip_setup = ( None, "eth1" ) )

print c._serial_cmd( "ls /" )[1]
c._serial.logout()
