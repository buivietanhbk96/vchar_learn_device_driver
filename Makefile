#KDIR = /home/vanh/Desktop/SVTT/linux-kernel/mach1/TI814X-LINUX-PSP-04.01.00.07/src/kernel/linux-04.01.00.07
EXTRA_CFLAGS =  -Wall
KDIR = /lib/modules/`uname -r`/build
obj-m        =  vchar_driver.o
all:
	make -C $(KDIR) M=`pwd`

clean:
	make -C $(KDIR) M=`pwd` clean
