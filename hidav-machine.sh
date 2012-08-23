#!/bin/bash
#
# HidaV machine changer script
# maintained by tfm <fromm@dresearch-fe.de>
#

this=`dirname $0`

function usage() {
    local current="$1"

    echo ""
    echo "   Current machine is: $current"
    echo ""
    echo "   Available machine types are"
    echo "   hidav-machine.sh ti81xx     - build HidaV for Texas Instruments' DaVinci DM814x / DM816x"
    echo "   hidav-machine.sh cedartrail - build HidaV for Intel Cedartrail (Atom N2x00 + NM10)"
    echo ""
}
# ---

function xtract_current() {
    local current=""
    local tmp=$(     grep -E '^[[:space:]]*MACHINE[?:[:space:]]*=' $this/build/conf/local.conf \
            | sed 's/.*=[^"]*"\([^"]*\)".*/\1/' ) 

    case "$tmp" in
        "hidav-ti81xx")   current="ti81xx";;
        "cedartrail")     current="cedartrail";;
        *)        echo "" >&2
                  echo "    ERROR: unsupported value '$tmp' for MACHINE field in configuration file at" >&2
                      echo "    $this/build/conf/local.conf" >&2
                      echo "    Supported values for MACHINE is 'hidav-ti81xx'." >&2 
                      echo "" >&2
                  return 1;;
    esac
    echo -n $current
    return 0
}
# ---

function set_new() {
    local tmp="$1"
    local new=""

    case "$tmp" in
        "ti81xx")      new="hidav-ti81xx";;
        "cedartrail")  new="cedartrail";;
        *)        echo "" >&2
                  echo "    ERROR: unsupported machine '$tmp'." >&2
                      echo -n "    Supported machine is 'ti81xx'." >&2;
                      echo "" >&2
                  return 1;;
    esac

    sed -i "s/^[[:space:]]*MACHINE[?:[:space:]]*=.*/MACHINE ?= \"$new\"/" \
            $this/build/conf/local.conf
    echo ""
    echo "    The target machine is now $tmp."
    echo ""

    return 0
}
# ---


machine=$(xtract_current) || exit 1

if [ "$#" -lt 1 ]; then
    usage $machine
    exit 0
fi

set_new "$@"
exit $?

