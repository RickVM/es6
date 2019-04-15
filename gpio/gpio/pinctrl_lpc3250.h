#ifndef PINCTRL_LPC3250_H_
#define PINCTRL_LPC3250_H_

#include <linux/device.h> 
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <mach/hardware.h>

#include "pinctrl.h"

extern struct Pinctrl pinctrl_port0;
extern struct Pinctrl pinctrl_port2;
extern struct Pinctrl pinctrl_port3;

// Function prototypes

// Convert CONF enum to the characters that they represent
char conf_to_char(CONF direction);

// Search the pin struct of the pin Number of Connector
const struct Pin* searchPin(const char* connector, int number);
CONF getDirection(struct Pin* pin);
int setDirection(struct Pin* pin, const char direction);
int getValue(struct Pin* pin);
int setValue(struct Pin* pin, int value);
int setMux(void);
int turnOffLcd(void);

#endif /*PINCTRL_LPC3250_H_*/
