ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-
KDIR := ~/felabs/sysdev/tinysystem/linux-2.6.34/

obj-m := src/adc.o

default: kernel

kernel:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(GCC) -C $(KDIR) M=$(PWD) -o $@ $^  modules 

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
