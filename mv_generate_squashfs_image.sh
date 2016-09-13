#!/bin/sh

cd buildroot
DEBUG_FLAG="yes"
ARMADA_PLATFORM=a3700_z
BR_DIR=`pwd`

cd ..
PRE_DIR=`pwd`
ATF_DIR=$PRE_DIR/arm-trusted-firmware-1.2/arm-trusted-firmware-1.2
UBOOT_DIR=$PRE_DIR/u-boot/u-boot-2015.01/
KERNEL_DIR=$PRE_DIR//linux-4.4.8/
cd $BR_DIR
echo "!=================================================================!"
echo "!=====================Starting to build uboot/kernel/rootfs=====================!"
echo "!=================================================================!"
make V=1

UBOOT_CUSTOM_REPO_VERSION=$(grep -w -e "BR2_TARGET_UBOOT_CUSTOM_REPO_VERSION" .config | sed -n 's/BR2_TARGET_UBOOT_CUSTOM_REPO_VERSION=//gp')

cd $ATF_DIR
echo "!====================================================================================!"
echo "!====================Entering $ATF_DIR====================!"
echo "!====================================================================================!"
export BL33="$UBOOT_DIR/u-boot.bin"
if [ "$DEBUG_FLAG"="yes" ]; then
	make DEBUG=1 USE_COHERENT_MEM=0 LOG_LEVEL=20 PLAT=$ARMADA_PLATFORM all fip
	cp $ATF_DIR/build/$ARMADA_PLATFORM/debug/flash-image.bin $BR_DIR/output/images
else
	make DEBUG=0 USE_COHERENT_MEM=0 LOG_LEVEL=20 PLAT=$ARMADA_PLATFORM all fip
	cp $ATF_DIR/build/ARMADA_PLATFORM/release/flash-image.bin $BR_DIR/output/images
fi
cd $BR_DIR/output/images
echo "!==============================================================================!"
echo "!====================Entering $BR_DIR/output/images====================!"
echo "!==============================================================================!"
echo "copy kernel file to here!!!!!!"
cp $KERNEL_DIR/arch/arm64/boot/Image.gz ./
cp $KERNEL_DIR/arch/arm64/boot/dts/marvell/armada-3720-db.dtb ./
$UBOOT_DIR/tools/mkimage -A arm64 -O linux -C gzip -T kernel -a 0x80000 -e 0x80000 -n 'Armada-3700 Firmware' -d ./Image.gz uImage
truncate -s %64K uImage
truncate -s %64K armada-3720-db.dtb
truncate -s 6M uImage
if  test -d "userdata";  then
	echo "Found userdata folder! turning it to jffs2 image."
else
	mkdir userdata
fi
cp $BR_DIR/user_files/* userdata/ -rf

mkfs.jffs2 --little-endian --eraseblock=0x10000 -n -d userdata --pad=0x80000 -o userdata1_jffs2.img
mkfs.jffs2 --little-endian --eraseblock=0x10000 -n -d userdata --pad=0x80000 -o userdata2_jffs2.img
cat armada-3720-db.dtb uImage rootfs.squashfs > a3700_upgrade.img
truncate -s 2M flash-image.bin
cat flash-image.bin userdata1_jffs2.img userdata2_jffs2.img armada-3720-db.dtb uImage rootfs.squashfs > a3700_rcvr.img

echo "                                 flash layout"
echo "    +--------------------+------------------+----------+-------------------+"
echo "    | U-Boot & env. var. | JFFS2 filesystem | FDT file | Kernel + Squashfs |"
echo "    +--------------------+------------------+----------+-------------------+"
echo "    |        2MB         | 512K   |  512K   |   64KB   |                   |"
echo "    +--------------------+------------------+----------+-------------------+"
echo "    0                    0x200000 |0x280000 0x300000   0x310000            |"
echo "    +--------------------+------------------+----------+-------------------+"

test `ls a3700_rcvr.img -l|awk '{print $5}'` -le 16777216 #16M
test `ls a3700_upgrade.img -l|awk '{print $5}'` -le 14155776 #16M-0x280000

md5sum a3700_rcvr.img > a3700_rcvr.img.md5
md5sum a3700_upgrade.img > a3700_upgrade.img.md5
du -h a3700_*.img




