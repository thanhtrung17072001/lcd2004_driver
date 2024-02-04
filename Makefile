obj-m += lcd2004.o

CROSS_TOOLCHAIN=~/Work/beagleboneblack/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
TARGET_ARCH=arm
KERNEL_DIR=~/Work/beagleboneblack/kernelbuildscripts/KERNEL
WORKSPACE_DIR=`pwd`

all:
	make ARCH=${TARGET_ARCH} CROSS_COMPILE=${CROSS_TOOLCHAIN} -C ${KERNEL_DIR} M=${WORKSPACE_DIR} modules

clean:	
	make -C ${KERNEL_DIR} M=${WORKSPACE_DIR} clean