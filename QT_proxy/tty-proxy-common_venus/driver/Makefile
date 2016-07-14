
# Comment/uncomment the following line to enable/disable debugging

#DEBUG = y

INPUT_SUPPORT = n

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DDEBUG -D_ksocket_debug_ # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += -DEXPORT_SYMTAB $(DEBFLAGS) -I$(LDDINC)

ifeq ($(INPUT_SUPPORT),y)
EXTRA_CFLAGS += -DINPUT_SUPPORT
endif

ifneq ($(KERNELRELEASE),)

ttyp-objs := kttyp.o ksocket.o
obj-m += ttyp.o

ifeq ($(INPUT_SUPPORT),y)
obj-m += skbd.o
obj-m += gtouch.o
endif

else

#  KERNELDIR ?= /home/test/s3c-linux-2.6.21
#  KERNELDIR ?= /home/xlongfeng/Desktop/linux-2.6.18.8
#  KERNELDIR ?= /home/xlongfeng/Desktop/linux-2.6.28
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules

endif

install:

uninstall:

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers .*.o.d modules.order

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
