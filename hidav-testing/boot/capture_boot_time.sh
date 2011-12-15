#!/bin/bash
#
# Helper script to log boot timings via serial port.
#
set -x
iface="/dev/ttyUSB1"
# use this with noisy boot: stop_at="Freeing init memory"
stop_at="omap_voltage_late_init"

[ $# -ge 1 ] && iface=$1
[ $# -ge 2 ] && lines=$2



function capture_boot() {
	lnr=0
    ( while read line; do 
      [ $lnr -lt 1 ] && echo -en " Got it.\nReading lines:" 1>&2
      lnr=$((lnr+1))
      printf "%3d %s %s\n" "$lnr" "`date +%s.%N`" "$line";
      echo -n "$lnr, " >&2 
      echo "$line" | grep -q "$stop_at" && break
      done
    ) < $iface 
}
# ---

function calc_boot_diffs() {
    awk 'BEGIN{                                                              
        last=0 } 
    {  
        if (last && $2) { 
            printf("%3f ", $1);
            system("echo -e \"scale=20\\n" $2 "-" last "\\n\" | bc 2>/dev/null | awk \"{printf \\\"%2.10f\\\", \\$0 }\""); 
            print "   " $0
        } 
        last=$2;
    }
    ' boot.log | awk '{ 
		if (line) 
			printf("%3d %10f %s\n", lnr, $2, line); 
		lnr  = $1; 
		line = ""; 
		for ( i=5; i<50; i++ ) 
			if($i) 
				line = line " " $i 
		}' 
}

#####################
####################

echo -n "Will read until \"$stop_at\". Waiting for traffic on $iface..." 1>&2
#echo "boot" >$iface
capture_boot >boot.log

echo -ne "\n\nDone. Post processing:"
calc_boot_diffs > boot-diffs.log
echo -n "boot-diffs.log, "

awk '{if (sum) { printf("%3d %10f  ", $1, sum); for(i=2; i<50; i++) if($i) printf " " $i; print "";} sum=sum+$2; }' boot-diffs.log >boot-diffs-summed.log
echo -n "boot-diffs-summed.log, "

sort -nk 3 boot-diffs-summed.log >boot-diffs-summed-sorted.log
echo -n "boot-diffs-summed-sorted.log, "
awk '{if (sum) { printf("%3d %10f  ", $1, sum); for(i=3; i<50; i++) if($i) printf " " $i; print "";} sum=sum+$3; }' boot-diffs-summed-sorted.log >boot-diffs-summed-sorted-summed.log
echo  "boot-diffs-summed-sorted-summed.log "
