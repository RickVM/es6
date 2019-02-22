ARCH=arm
GCC=/usr/local/xtools/arm-unknown-linux-uclibcgnueabi/bin/arm-unknown-linux-uclibcgnueabi-
DIR=src/user/
TARGET := readRTC

default: $(TARGET)

$(TARGET):
	$(GCC)gcc $(DIR)$(TARGET).c -o $(DIR)$(TARGET)

clean:
	rm $(DIR)/$(TARGET)
