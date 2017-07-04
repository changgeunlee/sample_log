KDIR := /lib/modules/4.8.0-56-generic/build

obj-m += netlinkKernel.o

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.cmd .module* modules* Module* .*.cmd .tmp*
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
