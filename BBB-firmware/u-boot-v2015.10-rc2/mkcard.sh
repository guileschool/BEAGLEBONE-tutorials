#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <drive>"
    exit 1;
fi

DRIVE=$1

dd if=/dev/zero of=$DRIVE bs=1024 count=1024

SIZE=`fdisk -l $DRIVE | grep Disk | grep bytes | awk '{print $5}'`

echo DISK SIZE - $SIZE bytes

CYLINDERS=`echo $SIZE/255/63/512 | bc`

echo CYLINDERS - $CYLINDERS

{
echo ,9,0x0C,*
echo ,,,-
} | sfdisk -D -H 255 -S 63 -C $CYLINDERS $DRIVE

sleep 1


if [ -x `which kpartx` ]; then
    NEWDISK=`kpartx -a ${DRIVE}`
    if [ -n "${NEWDISK}" ] ; then echo "no disk, exit"; exit; fi
fi

echo "SD카드를 분리후 다시 꼽아 주세요."
read -r -p "SD카드를 꼽으셨으면 아무키나 눌러주세요" key

export USBKEYS=($(
    grep -Hv ^0$ /sys/block/*/removable |
    sed s/removable:.*$/device\\/uevent/ |
    xargs grep -H ^DRIVER=sd |
    sed s/device.uevent.*$/size/ |
    xargs grep -Hv ^0$ |
    cut -d / -f 4
))

export STICK
case ${#USBKEYS[@]} in
    0 ) echo No USB Stick found; exit 0 ;;
    1 ) STICK=$USBKEYS ;;
    * )
    STICK=$(
    bash -c "$(
        echo -n  dialog --menu \
            \"Choose wich USB stick have to be installed\" 22 76 17;
        for dev in ${USBKEYS[@]} ;do
            echo -n \ $dev \"$(
                sed -e s/\ *$//g </sys/block/$dev/device/model
                )\" ;
            done
        )" 2>&1 >/dev/tty
    )
    ;;
esac

# now make partitions.
if [ -b /dev/${STICK}1 ]; then
    mkfs.vfat -F 32 -n "BOOT" /dev/${STICK}1
else
    echo "Cant find boot partition in /dev"
fi

if [ -b /dev/${STICK}2 ]; then

    mke2fs -j -L "Angstrom" /dev/${STICK}2
else
    echo "Cant find rootfs partition in /dev"
fi

## copy files
sudo mount /dev/${STICK}1 /media/mdsedu/BOOT1
sudo cp MLO u-boot.img uEnv.txt /media/mdsedu/BOOT1
sleep 1
sudo umount /media/mdsedu/BOOT1
