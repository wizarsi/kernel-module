obj-m += lab2-ioctl.o

build_user:
	gcc -Wall -Werror -pedantic-errors -o lab2-ioctl-user lab2-ioctl-user.c

build: build_user
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	sudo rmmod lab2-ioctl

install: build
	sudo insmod lab2-ioctl.ko