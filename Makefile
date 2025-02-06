obj-m := message_slot.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: module programs

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

programs:
	gcc -O3 -Wall -std=c11 -o message_sender message_sender.c
	gcc -O3 -Wall -std=c11 -o message_reader message_reader.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f message_sender message_reader
