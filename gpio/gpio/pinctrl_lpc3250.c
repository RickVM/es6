#include "pinctrl_lpc3250.h"

#define number_pins_port2 13

CONF getDirection(struct Registers* registers, int index) {
    // Not implemented yet
    CONF conf = disabled;
    return conf;
}

void setDirection(struct Registers* registers, int index) {
    // Not implemented yet
}

int getValue(unsigned long reg, int index) {
    // Not implemented yet
    return 0;
}

void setValue(struct Registers* reg, int index) {
    // Not implemented yet
}

int setMux(void) {
    return 0;
}

int turnOffLcd(void) {
    return 0;
}

pinctrl_port2 = {
    {   // Registers
        0x4002801C, // INP_STATE 
        0x40028020, // OUTP_SET
        0x40028024, // OUTP_CLR
        0, // OUTP_STATE
        0x40028010, // DIR_SET
        0x40028014, // DIR_CLR
        0x40028018// DIR_STATE
    }, 
    number_pins_port2, // Number of Pins
    { // Pins 
        { "", 0, 0, &pinctrl_port2 }, // P2.0
        { "", 0, 1, &pinctrl_port2 }, // P2.1
        { "", 0, 2, &pinctrl_port2 }, // P2.2
        { "", 0, 3, &pinctrl_port2 }, // P2.3
        { "", 0, 4, &pinctrl_port2 }, // P2.4
        { "", 0, 5, &pinctrl_port2 }, // P2.5
        { "", 0, 6, &pinctrl_port2 }, // P2.6
        { "", 0, 7, &pinctrl_port2 }, // P2.7
        { "", 0, 8, &pinctrl_port2 }, // P2.8
        { "", 0, 9, &pinctrl_port2 }, // P2.9
        { "", 0, 10, &pinctrl_port2 }, // P2.10
        { "", 0, 11, &pinctrl_port2 }, // P2.11
        { "", 0, 12, &pinctrl_port2 }, // P2.12
    }, 
    getDirection, 
    setDirection,
    getValue,
    setValue,
    setMux
};