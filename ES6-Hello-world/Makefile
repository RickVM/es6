ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-unknown-linux-uclibcgnueabi-
obj-m := src/kernel/helloWorld.o
KDIR := ~/felabs/sysdev/tinysystem/linux-2.6.34/

default: kernel

kernel:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(GCC) -C $(KDIR) M=$(PWD) -o BUILDIR/$@  modules 

userSpace:

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean