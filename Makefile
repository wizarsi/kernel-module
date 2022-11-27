obj-m += lab2-ioctl.o

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	sudo rmmod lab2-ioctl

install: build
	sudo insmod lab2-ioctl.ko