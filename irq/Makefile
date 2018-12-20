obj-m += keyboard_statistics.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
mount:
	sudo insmod keyboard_statistics.ko
	dmesg | tail -1
unmount:
	sudo rmmod keyboard_statistics.ko
	dmesg | tail -1
