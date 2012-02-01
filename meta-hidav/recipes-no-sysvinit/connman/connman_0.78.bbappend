# do not generate rc-links
PRINC := "${@int(PRINC) + 2}"
INITSCRIPT_NAME = "-f connman"
INITSCRIPT_PARAMS = "remove"
