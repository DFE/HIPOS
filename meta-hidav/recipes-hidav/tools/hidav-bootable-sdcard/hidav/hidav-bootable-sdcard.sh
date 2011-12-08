#! /bin/sh
# mk3PartSDCard.sh v0.3
# Licensed under terms of GPLv2
# found at http://processors.wiki.ti.com/index.php/How_to_Make_3_Partition_SD_Card

if ! [ -b "$1" ]; then
    echo -e "\nUsage: $0 <block device>   Generate bootable SD Card on <block device>."
    exit
fi

if ! whoami | grep -wq "root"; then
    echo -e "\nYou must be ROOT in order for this script to work.\nWait, I can fix this. I'll run  \" sudo $0 $@ \" for you.\n"
    sudo $0 $@
    exit
fi

echo -en "\nTHIS WILL ERASE ALL DATA ON $1. ARE YOU SURE? (y/N):"
read yn

case $yn in
    Y)  ;;
    y)  ;;
    *)  echo "Aborted."; exit
esac

DRIVE=$1

dd if=/dev/zero of=$DRIVE bs=1024 count=1024

SIZE=`fdisk -l $DRIVE | grep Disk | awk '{print $5}'`

echo DISK SIZE - $SIZE bytes

CYLINDERS=`echo $SIZE/255/63/512 | bc`

echo -e "----\nCreating Partitions\n--"

sfdisk -D -H 255 -S 63 -C $CYLINDERS $DRIVE << EOF
,9,0x0C,*
10,115,,-
126,,,-
EOF

echo -e "----\nFormatting FAT Partition\n--"
umount ${DRIVE}1
mkfs.vfat -F 32 -n "boot" ${DRIVE}1

echo -e "----\nFormatting EXT4 (root) Partition\n--"
umount ${DRIVE}2
mkfs.ext4 -L "rootfs" ${DRIVE}2

mkdir -p tmp_mnt
mount ${1}2 tmp_mnt
pushd tmp_mnt > /dev/null
tar xjvf ../hydraip-devimage-hidav.tar.bz2
popd > /dev/null
umount tmp_mnt
rm -rf tmp_mnt

echo -e "----\nCopying bootloader and kernel\n--"
mkdir -p tmp_mnt
mount ${1}1 tmp_mnt
cp MLO tmp_mnt/
cp u-boot-2nd.sd tmp_mnt/
cp MLO.nand tmp_mnt/
cp uImage-hidav.bin tmp_mnt/uImage
cp hydraip-image-hidav.squashfs tmp_mnt/hidav-root-fs.squashfs
umount tmp_mnt
rm -rf tmp_mnt

echo -e "----\nDONE."
