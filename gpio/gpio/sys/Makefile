ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-
obj-m := sys/gpio.o
KDIR := ~/felabs/sysdev/tinysystem/linux-2.6.34/
DIR= src/user/
TARGET := blabla

default: kernel

kernel:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(GCC) -C $(KDIR) M=$(PWD) -o $@ $^  modules 
	rm sys/*.o 
	rm sys/.gpio**
	rm sys/*.mod.c

$(TARGET):
	$(GCC)gcc $(DIR)$(TARGET).c -o $(DIR)$(TARGET)

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean