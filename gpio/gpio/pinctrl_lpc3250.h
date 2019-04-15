#ifndef PINCTRL_LPC3250_H_
#define PINCTRL_LPC3250_H_

#include "pinctrl.h"

#define number_pins_port2 13
#define P2_MUX_SET 0x40028028
#define EMC_D_SEL 3

struct Pin* searchPin(const char* connector, int nummer) {
    return NULL;
}

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
    return 1;
}

void setValue(struct Registers* reg, int index) {
    // Not implemented yet
}

int setMux(void) {
    unsigned long* memAddr = 0;
    memAddr = io_p2v(P2_MUX_SET);
    *memAddr |= (1UL << EMC_D_SEL);
    return 0;
}

int turnOffLcd(void) {
    return 0;
}

struct Pinctrl pinctrl_port0;
struct Pinctrl pinctrl_port1;

struct Pinctrl pinctrl_port2 = {
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
            { "J3", 47, 0, &pinctrl_port2 }, // P2.0
            { "J3", 56, 1, &pinctrl_port2 }, // P2.1
            { "J3", 48, 2, &pinctrl_port2 }, // P2.2
            { "J3", 57, 3, &pinctrl_port2 }, // P2.3
            { "J3", 49, 4, &pinctrl_port2 }, // P2.4
            { "J3", 58, 5, &pinctrl_port2 }, // P2.5
            { "J3", 50, 6, &pinctrl_port2 }, // P2.6
            { "J3", 45, 7, &pinctrl_port2 }, // P2.7
            { "J1", 49, 8, &pinctrl_port2 }, // P2.8
            { "J1", 50, 9, &pinctrl_port2 }, // P2.9
            { "J1", 51, 10, &pinctrl_port2 }, // P2.10
            { "J1", 52, 11, &pinctrl_port2 }, // P2.11
            { "J1", 53, 12, &pinctrl_port2 }, // P2.12
        }, 
        getDirection, 
        setDirection,
        getValue,
        setValue,
        setMux
    } ;

struct Pinctrl pinctrl_port3;

#endif /*PINCTRL_LPC3250_H_*/
