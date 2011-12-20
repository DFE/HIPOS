#!/bin/ash
#
# mount ubifs application file system and aufs writeable overlay
#

#
# Prerequisites: /dev and /proc are mounted. Maybe /sys is required, too.
#

# exit upon error
set -e

application_fs_mtd="/dev/mtd5"
appfs_ubi_device="/dev/ubi0"
appfs_ubi_volume="${appfs_ubi_device}_0"

writeable_overlays_mountpoint="/media/writeable-overlays"
readonly_root_mountpoint="media/readonly-rootfs"    # <- DO NOT add a leading "/". It is missing by purpose.
writeable_root_mountpoint="/media/writeable-rootfs"

modprobe ubifs

if mount | grep -qE "aufs on / type aufs" ; then
    logger -s -p syslog.notice -t rootfs-overlay \
        "The root filesystem already is AUFS. ABORTING."
    exit 1
fi

# attach MTD, format to UBI if device is "raw"
if ! ubiattach -O 2048 -p $application_fs_mtd ; then
    logger -s -p syslog.notice -t rootfs-overlay \
        "Initialising empty UBIFS at $application_fs_mtd."
    ubiformat -O 2048 $application_fs_mtd
    ubiattach -O 2048 -p $application_fs_mtd
fi

# check for ubi volume; create dynamic volume if not present
if ! test -e $appfs_ubi_volume ; then
    logger -s -p syslog.notice -t rootfs-overlay \
        "Creating dynamic UBIFS partition 'application_fs' on $appfs_ubi_device."
    ubimkvol -m -N application_fs $appfs_ubi_device
fi

# mount ubifs to get access to the overlay directories
mkdir -p $writeable_overlays_mountpoint
mount -t ubifs $appfs_ubi_volume $writeable_overlays_mountpoint

if ! test -d ${writeable_overlays_mountpoint}/rootfs/0 ; then
    logger -s -p syslog.notice -t rootfs-overlay \
        "Creating writeable overlays directory."

    # discard stale overlays >0 if #0 is not present
    rm -rf ${writeable_overlays_mountpoint}/rootfs/
    mkdir -p ${writeable_overlays_mountpoint}/rootfs/0/
fi

# assemble aufs writeable overlay mount string from snapshot directories
aufs_mnt_opt=""
cd ${writeable_overlays_mountpoint}/rootfs
for dir in `find -type d -maxdepth 1 | sort -n -t / -k 2 -r | cut -b 3-` ; do

    overlay="${writeable_overlays_mountpoint}/rootfs/$dir"

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
mkdir -p ${writeable_root_mountpoint}
mount -t aufs -o "$aufs_mnt_opt" aufs ${writeable_root_mountpoint}

#make sure our important mount points exist in the writeable overlay
mkdir -p ${writeable_root_mountpoint}/${readonly_root_mountpoint}
mkdir -p ${writeable_root_mountpoint}/${writeable_overlays_mountpoint}
 
# bind-mount all other mounted FS to ${writeable_root_mountpoint} 
cd ${writeable_root_mountpoint}
old=""
for fs in `mount | cut -d " " -f 3 | sort` ; do
    # skip sub-mounts
    [ ! -z "$old" ] && echo "$fs" | grep -qE "^\\$old" && continue
    # skip root and writeable overlay mount
    [ "$fs" = "/" ] && continue
    echo "${writeable_root_mountpoint}" | grep -qE "^\\$fs" && continue

    mount --move "$fs" "${writeable_root_mountpoint}${fs}"

    old="$fs"
done

# now make aufs the new root
mkdir -p ${readonly_root_mountpoint}
pivot_root . ${readonly_root_mountpoint}

exec <dev/console >dev/console 2>&1


