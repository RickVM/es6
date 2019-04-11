ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-linux-
obj-m := led.o
KDIR := ~/felabs/sysdev/tinysystem/linux-2.6.34/
DIR= src/user/
TARGET := user-led

default: kernel

kernel:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(GCC) -C $(KDIR) M=$(PWD) -o $@ $^  modules 
	rm *.o 

$(TARGET):
	$(GCC)gcc $(DIR)$(TARGET).c -o $(DIR)$(TARGET)

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm $(DIR)/$(TARGET)