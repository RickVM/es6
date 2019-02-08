#include "i2c_ext.h"

inline __s32 i2c_select_slave(int fd, __u8 addr)
{
    return ioctl(fd, I2C_SLAVE, (__u16)addr);
}
