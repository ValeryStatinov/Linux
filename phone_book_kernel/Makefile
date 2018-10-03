obj-m += phone_book_kernel.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc addContact.c -o add
	gcc getContact.c -o get
	gcc delContact.c -o del 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm add get del
unmount:
	sudo rm -rf /dev/phone_book_kernel
	sudo rmmod phone_book_kernel.ko
mount: 
	sudo insmod phone_book_kernel.ko
	sudo mknod /dev/phone_book_kernel c 243 0