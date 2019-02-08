#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "i2c_ext.h"
#include "pca9532.h"

#define DEVICE "/dev/i2c-0"

int main(int argc, char* argv[])
{
    int dev;

    /* Open the i2c master device */
    if ((dev = open(DEVICE, O_RDWR)) < 0)
    {
        perror("Can't open device");
        return 1;
    }

    /* Select the right slave for our master to talk to */
    if (i2c_select_slave(dev, PCA9532_ADDRESS))
    {
        perror("Can't select device");
        close(dev);
        return 1;
    }

    /* Setup the states */
    unsigned int i;
    __u8 states[16][2] = {
        { PCA9532_REGISTER_LS2, PCA9532_SET_LED0(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS2, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS2, PCA9532_SET_LED1(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS2, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS2, PCA9532_SET_LED2(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS2, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS2, PCA9532_SET_LED3(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS2, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS3, PCA9532_SET_LED0(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS3, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS3, PCA9532_SET_LED1(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS3, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS3, PCA9532_SET_LED2(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS3, PCA9532_LEDMODE_OFF },
        { PCA9532_REGISTER_LS3, PCA9532_SET_LED3(PCA9532_LEDMODE_ON) },
        { PCA9532_REGISTER_LS3, PCA9532_LEDMODE_OFF },
    };
    
    for (i = 0; i < 16; i++)
    {
        if (i2c_smbus_write_byte_data(dev, states[i][0], states[i][1]))
        {
            printf("Couldn't set LED 0x%02X on register 0x%02X\n", (int)states[i][1],
                   (int)states[i][0]);
        }
        else
        {
            printf("Set LED 0x%02X on register 0x%02X\n", (int)states[i][1],
                   (int)states[i][0]);
        }

        usleep(500000);
    }

    close(dev);

    return 0;
}
