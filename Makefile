EXTRA_CFLAGS =  -Wall
KDIR = /lib/modules/`uname -r`/build
obj-m        =  vchar_driver.o
all:
	make -C $(KDIR) M=`pwd`

clean:
	make -C $(KDIR) M=`pwd` clean
