ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-unknown-linux-uclibcgnueabi-
obj-m := src/helloWorld.o #src/kernel/peek-poke-sys.o
KDIR := ~/felabs/sysdev/tinysystem/linux-2.6.34/
DIR=src/user/
TARGET := helloWorld#user-peek-poke-sys

default: kernel $(TARGET)

kernel:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(GCC) -C $(KDIR) M=$(PWD) -o $@ $^  modules 
	rm src/kernel/*.o 

$(TARGET):
	$(GCC)gcc $(DIR)$(TARGET).c -o $(DIR)$(TARGET)

laptop:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules clean # Fix bug where it first compiles, then cleans
	rm $(DIR)/$(TARGET)
