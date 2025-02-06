obj-m := message_slot.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	sudo insmod message_slot.ko
	sudo mknod /dev/slot0 c 235 0
	sudo chmod 666 /dev/slot0

uninstall:
	sudo rm -f /dev/slot0
	sudo rmmod message_slot

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
