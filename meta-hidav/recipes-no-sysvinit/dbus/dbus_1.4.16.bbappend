# do not generate rc-links
PRINC := "${@int(PRINC) + 2}"
INITSCRIPT_NAME = "-f dbus-1"
INITSCRIPT_PARAMS = "remove"
