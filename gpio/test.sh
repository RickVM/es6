!#/bin/sh 

echo "Loading kernels"

insmod /home/gpio-sys.ko
insmod /home/gpio-dev.ko

echo "Turning on leds!"

echo J1 51 O > /sys/kernel/gpio/config
echo J1 52 O > /sys/kernel/gpio/config
echo J1 53 O > /sys/kernel/gpio/config

echo J1 51 1 > /dev/gpio-set 
echo J1 52 1 > /dev/gpio-set 
echo J1 53 1 > /dev/gpio-set 

echo "Setting joystick for read!"

echo J3 47 I > /sys/kernel/gpio/config
echo J3 56 I > /sys/kernel/gpio/config
echo J3 48 I > /sys/kernel/gpio/config
echo J3 57 I > /sys/kernel/gpio/config
echo J3 49 I > /sys/kernel/gpio/config
