#!/bin/sh -

# find root device
ROOT_DEVICE=$(findmnt / -o source -n)
# prune root device (for example UUID)
ROOT_DEVICE=$(realpath ${ROOT_DEVICE})
ROOT_PART_NAME=$(echo "$ROOT_DEVICE" | cut -d "/" -f 3)
DEVICE_NAME=$(echo /sys/block/*/"${ROOT_PART_NAME}" | cut -d "/" -f 4)
DEVICE="/dev/${DEVICE_NAME}"

PADDING=""
case $DEVICE_NAME in
    mmcblk*)
        PADDING="p"
        USER_PART_NUM=`ls /sys/block/${DEVICE_NAME} | grep "${DEVICE_NAME}" | grep -v "mmcblk[0-9]\{1,\}boot[0-9]$" | wc -l`
        ;;
    sd*)
        PADDING=""
        USER_PART_NUM=`ls /sys/block/${DEVICE_NAME} | grep "${DEVICE_NAME}" | wc -l`
        ;;
esac

BOOT_PART_NUM=`expr $USER_PART_NUM - 2`

case "$1" in
    start)
        [ -d /boot ] || mkdir -p /boot
        [ -e ${DEVICE}${PADDING}${BOOT_PART_NUM} ] && mount ${DEVICE}${PADDING}${BOOT_PART_NUM} /boot
        [ -d /usr/local ] || mkdir -p /usr/local
        [ -e ${DEVICE}${PADDING}${USER_PART_NUM} ] && mount ${DEVICE}${PADDING}${USER_PART_NUM} /usr/local
        ;;
    stop)
        umount ${DEVICE}${PADDING}${BOOT_PART_NUM}
        umount ${DEVICE}${PADDING}${USER_PART_NUM}
        ;;
    *)
        echo "Usage: $0 [start|stop]"
        ;;
esac
