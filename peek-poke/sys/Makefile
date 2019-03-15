ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-unknown-linux-uclibcgnueabi-
obj-m := src/kernel/peek-poke.o #src/helloWorld.o 
KDIR := ~/felabs/sysdev/tinysystem/linux-2.6.34/
DIR=src/kernel/
TARGET := peek-poke

default: kernel $(TARGET)

kernel:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(GCC) -C $(KDIR) M=$(PWD) -o $@ $^  modules 
	rm src/kernel/*.o 

#$(TARGET):
	#$(GCC)gcc $(DIR)$(TARGET).c -o $(DIR)$(TARGET)

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm $(DIR)/$(TARGET) # a
