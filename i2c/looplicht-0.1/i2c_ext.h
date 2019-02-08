/*
 * Handy little extensions to the "normal" i2c lib.
 */

#include "lmsensors_i2c-dev.h"
#include <fcntl.h>
#include <unistd.h>

#ifndef I2C_EXT_H
#define I2C_EXT_H

/* Force 8-bit addresses because 10-bit is broken in Linux atm */
inline __s32 i2c_select_slave(int fd, __u8 addr);

#endif
