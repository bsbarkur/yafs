
#Comment this line if ready for release.
DEBUG = y

ifeq ($(DEBUG),y)
	DBGFLAGS = -O -g -DMYFS_DEBUG
else
	DBGFLAGS = -O2
endif

EXTRA_CFLAGS += $(DBGFLAGS) 
#Compile from within kernel framework if defined.
ifneq ($(KERNELRELEASE),)
	obj-m := myfsd.o

myfsd-objs := myfs.o myfs_inode.o myfs_namei.o myfs_dir.o

#otherwise called directly from command line 
else
	KERNELDIR :=  ~/linux-2.6.27.7-9/
	PWD := $(shell pwd)

default:
	$(MAKE) -C  $(KERNELDIR) M=$(PWD) modules

endif 

