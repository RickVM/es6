DIR=src/user/
ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-unknown-linux-uclibcgnueabi-
TARGET := readRTC

default:
	gcc $(DIR)$(TARGET).c -o $(DIR)$(TARGET)


arm:
	$(GCC)gcc $(DIR)$(TARGET).c -o $(DIR)$(TARGET)


clean:
	rm $(DIR)/$(TARGET)