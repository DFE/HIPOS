# do not generate rc-links
PRINC := "${@int(PRINC) + 2}"
INITSCRIPT_NAME = "-f dnsmasq"
INITSCRIPT_PARAMS = "remove"
