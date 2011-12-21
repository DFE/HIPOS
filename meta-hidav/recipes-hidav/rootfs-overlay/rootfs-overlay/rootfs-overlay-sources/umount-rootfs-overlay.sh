#!/bin/ash
#
# Writeable overlay un-mount script; part of roofs-overlay package.
#
# Copyright (C) 2011 DResearch Fahrzeugelektronik GmbH
# Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#
#
# This script will unmount an AUFS root partition and put you back
# on the "original" root fs.
#

# exit upon error
set -e
cd /

# prerequisites check 
test -d ${overlays_data_mountpoint} ${pivot_root_mountpoint} ${original_root_mountpoint} ${original_root_mountpoint}/${pivot_root_mountpoint}
test -e ${appfs_ubi_volume}

if !  grep 'aufs / aufs' /proc/mounts ; then
    logger -s -p syslog.warn -t rootfs-overlay \
        "Unable to unmount rootfs-overlay: No AUFS rootfs mount listed in /proc/mounts."
    exit 1
fi

if !  grep -E "[^ ]* ${original_root_mountpoint}" /proc/mounts ; then
    logger -s -p syslog.warn -t rootfs-overlay \
        "Unable to find original root fs in /proc/mounts (expected to be mounted at ${original_root_mountpoint})"
    exit 1
fi

# Source the original configuration, i.e. the settings mount-rootfs-overlay used to
# set up the overlay. This is the configuration in the original root fs.
source ${original_root_mountpoint}/etc/default/rootfs-overlay

# make original root mount writeable; ignore result. 
# I.e. we don't care if it was a readonly fs originally and therefore cannot be mounted r/w.
mount -o remount,rw ${original_root_mountpoint} || \
        logger -s -p syslog.warn -t rootfs-overlay \
            "Failed to remount original root at ${original_root_mountpoint} read-write. Ignoring and continuing anyway."

# move all FS mounts of the overlay back to the original root
cd ${original_root_mountpoint}
old=""
for fs in `mount | cut -d " " -f 3 | sort` ; do
    # skip sub-mounts
    [ ! -z "$old" ] && echo "$fs" | grep -qE "^\\$old" && continue
    # skip overlay root and original root
    [ "$fs" = "/" ] && continue
    echo "${original_root_mountpoint}" | grep -qE "^\\$fs" && continue

    mount --move "$fs" "${original_root_mountpoint}${fs}" || \
        logger -s -p syslog.warn -t rootfs-overlay \
            "Failed to move-mount ${fs} to ${original_root_mountpoint}${fs}. Ignoring and continuing anyway."

    old="$fs"
done

# now switch back to the original root;
#  remove leading "/" to make old_root work across environments. See man pivot_root.
pivot_root . ${pivot_root_mountpoint#/}

# finally get rid of the overlay fs
umount ${pivot_root_mountpoint}

exec <dev/console >dev/console 2>&1


