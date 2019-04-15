ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-
obj-m := dev/gpio.o sys/gpio_sys.o
mymodule-objs := pinctrl_lpc3250.o
KDIR := ~/felabs/sysdev/tinysystem/linux-2.6.34/
DIR= src/user/
TARGET := user-gpio

default: kernel

kernel:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(GCC) -C $(KDIR) M=$(PWD) -o $@ $^  modules 
	rm dev/*.o 
	rm dev/.gpio**
	rm dev/*.mod.c
	rm sys/*.o 
	rm sys/.gpio_sys**
	rm sys/*.mod.c

$(TARGET):
	$(GCC)gcc $(DIR)$(TARGET).c -o $(DIR)$(TARGET)

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm $(DIR)/$(TARGET)