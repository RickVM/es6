ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-
KDIR := ~/felabs/sysdev/tinysystem/linux-2.6.34/

obj-m := gpio-dev.o gpio-sys.o

gpio-dev-objs := dev/gpio.o pinctrl_lpc3250.o
gpio-sys-objs := sys/gpio.o pinctrl_lpc3250.o

default: kernel

kernel:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(GCC) -C $(KDIR) M=$(PWD) -o $@ $^  modules 
	rm dev/*.o 
	rm dev/.gpio**
	rm dev/*.mod.c
	rm sys/*.o 
	rm sys/.gpio_sys**
	rm sys/*.mod.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
