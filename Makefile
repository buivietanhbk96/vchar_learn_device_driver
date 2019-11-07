EXTRA_CFLAGS =  -Wall
KDIR = /lib/modules/`uname -r`/build
obj-m        =  vchar_driver.o
all: ko app_test

ko:
	make -C $(KDIR) M=`pwd`

app_test_SOURCES:
	app_test.c

clean:
	make -C $(KDIR) M=`pwd` clean
	rm app_test
