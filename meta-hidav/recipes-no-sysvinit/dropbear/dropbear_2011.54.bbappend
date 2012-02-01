# do not generate rc-links
PRINC := "${@int(PRINC) + 2}"
INITSCRIPT_NAME = "-f dropbear"
INITSCRIPT_PARAMS = "remove"
