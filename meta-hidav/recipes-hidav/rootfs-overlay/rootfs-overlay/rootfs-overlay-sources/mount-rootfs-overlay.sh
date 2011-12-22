#!/bin/ash
#
# Writeable overlay mount script; part of roofs-overlay package.
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
# This script will mount a writeable AUFS overlay on top of /.
# First te script prepares, and, if necessary, formats an UBI / UBIFS partition
# which will serve as aufs' write backend.
# Then all filesystems mounted into the original root will be moved
# to the new root. Finally the script will pivot_root into the new AUFS overlay,
# leaving the original root read-only at ${original_root_mountpoint}.
#
# The script supports many layers. Each layer is a directory in UBIFS.
#
# Prerequisites: /dev and /proc are mounted.
#

# exit upon error
set -e
cd /

# prerequisites check 
source /etc/default/rootfs-overlay
modprobe ubifs
mkdir -p ${overlays_data_mountpoint} ${pivot_root_mountpoint}
test -e /proc/mounts
test -e ${application_fs_mtd}

# attach MTD, format to UBI if device is "raw"
if ! ubinfo ${appfs_ubi_device} >/dev/null 2>&1 && ! ubiattach -p $application_fs_mtd ; then
    logger -s -p syslog.notice -t rootfs-overlay \
        "Initialising empty UBIFS at $application_fs_mtd."
    ubiformat $application_fs_mtd
    ubiattach -p $application_fs_mtd
fi

# check for ubi volume; create dynamic volume if not present
if ! ubinfo ${appfs_ubi_volume} >/dev/null 2>&1 ; then
    logger -s -p syslog.notice -t rootfs-overlay \
        "Creating dynamic UBIFS partition 'application_fs' on $appfs_ubi_device."
    ubimkvol -m -N application_fs $appfs_ubi_device
fi

# mount ubifs to get access to the overlay directories
mount -t ubifs $appfs_ubi_volume $overlays_data_mountpoint

if ! test -d ${overlays_data_mountpoint}/rootfs/0 ; then
    logger -s -p syslog.notice -t rootfs-overlay \
        "Creating writeable overlays directory."

    # discard stale overlays >0 if #0 is not present
    rm -rf ${overlays_data_mountpoint}/rootfs/
    mkdir -p ${overlays_data_mountpoint}/rootfs/0/
fi

# assemble aufs writeable overlay mount string from snapshot directories
aufs_mnt_opt=""
cd ${overlays_data_mountpoint}/rootfs
for dir in `find -type d -maxdepth 1 | sort -n -t / -k 2 -r | cut -b 3-` ; do

    overlay="${overlays_data_mountpoint}/rootfs/$dir"

    # the last overlay (which is processed first in this loop)
    # is r/w, all the others are r/o
    if test -z "$aufs_mnt_opt" ; then
        aufs_mnt_opt="br:${overlay}=rw"
    else
        aufs_mnt_opt="${aufs_mnt_opt}:${overlay}=ro"
    fi
done

# finally add fs root (which is a readonly fs), the first overlay
aufs_mnt_opt="${aufs_mnt_opt}:/=rr"

#mount aufs
mount -t aufs -o "$aufs_mnt_opt" aufs ${pivot_root_mountpoint}

#make sure our important mount points exist in the writeable overlay
mkdir -p ${pivot_root_mountpoint}/${original_root_mountpoint}
mkdir -p ${pivot_root_mountpoint}/${overlays_data_mountpoint}
 
# move all other mounted FS to ${pivot_root_mountpoint} 
cd ${pivot_root_mountpoint}
old=""
for fs in `mount | cut -d " " -f 3 | sort` ; do

    # TODO: Use /proc/mounts and a clever grep -E | sort instead of this to handle special mounts
    # skip root fs and sub-mounts (mounts below other mounts)
    [ "$fs" = "/" ] && continue
    [ ! -z "$old" ] && echo "$fs" | grep -qE "^\\$old" && continue

    # skip writeable overlay, original root (if present), and overlay data mounts
    echo "${pivot_root_mountpoint}" | grep -qE "^\\$fs" && continue
    echo "${original_root_mountpoint}" | grep -qE "^\\$fs" && continue
    echo "${overlays_data_mountpoint}" | grep -qE "^\\$fs" && continue
    # TODO ends

    mount --move "$fs" "${pivot_root_mountpoint}${fs}"

    old="$fs"
done

# now make aufs the new root; 
#  remove leading "/" to make this a relative pathname (see man pivot_root)
pivot_root . ${original_root_mountpoint#/}

exec chroot . sh -lc "
    mount -o remount,ro ${original_root_mountpoint} " \
    <dev/console >dev/console 2>&1




