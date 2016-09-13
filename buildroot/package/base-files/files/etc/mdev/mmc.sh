#!/bin/sh

MDEVLEN=$(expr length $MDEV)
MDEVLENINDEX=$((MDEVLEN-2))
MDIR=/tmp/mnt/sdcard-${MDEV:$MDEVLENINDEX}

mkdir -p $MDIR
echo "$ACTION $MDEV to/from $MDIR">>/tmp/hot_plug_log

if [ "$ACTION" = "add" ];
then
	ismount=$(mount | grep -c $MDIR)
	if [ "$ismount" -eq "1" ]; then
		echo "$MDIR have mounted!!!">>/tmp/hot_plug_log
		exit 0
	fi

	# At mdev -s state during boot.
	if [ -z $DEVTYPE ];
	then
		COUNT=`find /dev -name "mmcblk*" | wc -l`
		if [ $COUNT -gt 1 ];
		then
			umount $MDIR > /dev/null 2>&1
		fi
		# Device has partition, not mount at disk node
	elif [ "$DEVTYPE" = "disk" -a $NPARTS -gt 0 ];
	then
		echo "DEVTYPE:$DEVTYPE is invalid!!!">>/tmp/hot_plug_log
		exit 0
	fi

#	exec 1>/dev/console
#	exec 2>/dev/console

	echo "Auto mount SDCARD(/dev/$MDEV) to $MDIR">>/tmp/hot_plug_log
	mount /dev/$MDEV $MDIR
	ismount=$(mount | grep -c $MDIR)
	if [ "$ismount" -eq "0" ]; then
		echo "mount UDISK(/dev/$MDEV) to $MDIR again>>>>">>/tmp/hot_plug_log
		mount /dev/$MDEV $MDIR
		ismount=$(mount | grep -c $MDIR)
		if [ "$ismount" -eq "0" ]; then
			rm -rf $MDIR
			echo "mount /dev/$MDEV to $MDIR failed!!!!!!!">>/tmp/hot_plug_log
			exit 1
		fi
	fi

	if [ -e $MDIR/sntest ]; then
		chmod +x $MDIR/sntest
		cd $MDIR/
		killall sntest
		./sntest
	fi
else
	echo "umount SDCARD from $MDIR">>/tmp/hot_plug_log
	umount -l $MDIR
	rm -rf $MDIR
fi
