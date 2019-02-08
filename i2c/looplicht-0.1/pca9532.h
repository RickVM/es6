#ifndef PCA9532_H
#define PCA9532_H

/* The I2C address of our chip */
#define PCA9532_ADDRESS 0x60

/* LED power modes */
#define PCA9532_LEDMODE_OFF  0x00
#define PCA9532_LEDMODE_ON   0x01
#define PCA9532_LEDMODE_PWM0 0x02 /* PWM unused for now */
#define PCA9532_LEDMODE_PWM1 0x03 /* But hey, what the heck */

/* Registers in which the LED controls live */
#define PCA9532_REGISTER_LS0 0x06
#define PCA9532_REGISTER_LS1 0x07
#define PCA9532_REGISTER_LS2 0x08
#define PCA9532_REGISTER_LS3 0x09

/* Defines mode to LED with certain offset
 * Every LED register has four LED controls
 */
#define PCA9532_SET_LED0(mode) (mode << 0)
#define PCA9532_SET_LED1(mode) (mode << 2)
#define PCA9532_SET_LED2(mode) (mode << 4)
#define PCA9532_SET_LED3(mode) (mode << 6)

#endif
