#!/bin/bash
# vim:set ts=4 sw=4 noexpandtab:
#
# \brief  BCC device info reader
# \description This script creates oder updates DEVID_FILE from BCC info. An update is
#              not executed if the serial number from BCC is invalid but the info on 
#              local root is valid. The script can be sourced to set all variables in DEVID_FILE.
# \author Ralf Schröder
#
# (C) 2013 DResearch Fahrzeugelektonik GmbH


#set -x

NAME="$0"
BCTRL_DEV="/dev/serial/by-path/platform-orion-ehci.0-usb-0:1.4:1.1-port0"
DRBCC_BIN="/usr/bin/drbcc"
DEVID_FILE="/etc/hipos/hipos-devid"
TMP_DEVID_FILE="/tmp/hipos-devid"

loggerANDstdoutError()
{
	echo "E: $@" 2>&1
	logger -t ${NAME} -p user.err "$@"
}

if [ ! -x "$DRBCC_BIN" ]; then
	loggerANDstdoutError "${DRBCC_BIN} not found"
	exit 1
fi

serial_valid()
{
	case $1 in
		[0-9][0-9][0-9][0-9]-[0-9]-[0-9][0-9][0-9][0-9][0-9]) return 0 ;;
		*) return 1 ;;
	esac
}


fetch_info()
{
	# fetch device info file from BCTRL
	local resp=`${DRBCC_BIN} --dev=${BCTRL_DEV} --cmd="gfiletype 0x50,${TMP_DEVID_FILE}" 2>&1`
	if [ ! -f ${TMP_DEVID_FILE} ]; then 
		loggerANDstdoutError "No device identity info file available in BCTRL, drbcc resp: \'$resp\'"
		if [ ! -e ${DEVID_FILE} ]; then
			# create the minimal file to avoid later problems
			echo "device=hipos" > ${DEVID_FILE}
		fi
	else
		echo "device=hipos" >> ${TMP_DEVID_FILE}
		if [ -e ${DEVID_FILE} ]; then 
			. ${TMP_DEVID_FILE}
			if ! serial_valid "${serial}"; then
				test -e $DEVID_FILE  && . $DEVID_FILE 
				if serial_valid "${serial}"; then
					loggerANDstdoutError "serial number from info file available in BCTRL is invalid (old is ${serial}, drbcc resp: \'${resp}\')"
					return 1
				fi
			fi
			# move only if different to avoid massive flash writing 
			diff -abq ${TMP_DEVID_FILE} ${DEVID_FILE} > /dev/null || mv -f ${TMP_DEVID_FILE} ${DEVID_FILE}
		else    
			mv -f ${TMP_DEVID_FILE} ${DEVID_FILE}
		fi      
	fi
}

# access the actual device config
# and actualize...
fetch_info

# check the link in /etc used by drunits
if [ ! -f /etc/hydraip-devid ]; then ln -s $DEVID_FILE /etc/hydraip-devid; fi

