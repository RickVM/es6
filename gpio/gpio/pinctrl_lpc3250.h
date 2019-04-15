#ifndef PINCTRL_LPC3250_H_
#define PINCTRL_LPC3250_H_

#include "pinctrl.h"

#define number_pins_port0 8
#define number_pins_port2 13
#define number_pins_port3 4

#define P2_MUX_SET 0x40028028
#define EMC_D_SEL 3
#define LCD_CFG 0x40004054
#define HCLK_ENABLE 5

struct Pin* searchPin(const char* connector, int nummer) {
    return NULL;
}

CONF getDirection(struct Pin* pin) {
    // Not implemented yet
    CONF conf = disabled;
    return conf;
}

int setDirection(struct Pin* pin, const char direction) {
    // Not implemented yet
    switch (direction) {
        case 'O':
            break;
        case 'I':
            break;
        default:
            break;
    }
    return 0;
}

int getValue(struct Pin* pin) {
    // Not implemented yet
    return 1;
}

int setValue(struct Pin* pin, int value) {
    // Not implemented yet
}

int setMux(void) {
    unsigned long* memAddr = 0;
    memAddr = io_p2v(P2_MUX_SET);
    *memAddr |= (1UL << EMC_D_SEL);
    return 0;
}

int turnOffLcd(void) {
    unsigned long* memAddr = 0;
    memAddr = io_p2v(LCD_CFG);
    *memAddr |= (1UL << HCLK_ENABLE);
    return 0;
}

struct Pinctrl pinctrl_port0 = {
    {
        0x40028040, // INP_STATE
        0x40028044, // OUTP_SET
        0x40028048, // OUTP_CLR
        0x4002804C, // OUTP_STATE
        0x40028050, // DIR_SET
        0x40028054, // DIR_CLR
        0x40028058  // DIR_STATE
    },
    number_pins_port0,
    {
        {"J3", 40, 0, &pinctrl_port0},  // P0.0
        {"J2", 24, 1, &pinctrl_port0},  // P0.1
        {"J2", 11, 2, &pinctrl_port0},  // P0.2
        {"J2", 12, 3, &pinctrl_port0},  // P0.3
        {"J2", 13, 4, &pinctrl_port0},  // P0.4
        {"J2", 14, 5, &pinctrl_port0},  // P0.5
        {"J3", 33, 6, &pinctrl_port0},  // P0.6
        {"J1", 27, 7, &pinctrl_port0},  // P0.7
    },
    getDirection,
    setDirection,
    getValue,
    setValue,
    turnOffLcd
};

struct Pinctrl pinctrl_port2 = {
    {   // Registers
        0x4002801C, // INP_STATE 
        0x40028020, // OUTP_SET
        0x40028024, // OUTP_CLR
        0x0,        // OUTP_STATE
        0x40028010, // DIR_SET
        0x40028014, // DIR_CLR
        0x40028018  // DIR_STATE
    }, 
    number_pins_port2, // Number of Pins
    { // Pins 
        { "J3", 47, 0, &pinctrl_port2 },    // P2.0
        { "J3", 56, 1, &pinctrl_port2 },    // P2.1
        { "J3", 48, 2, &pinctrl_port2 },    // P2.2
        { "J3", 57, 3, &pinctrl_port2 },    // P2.3
        { "J3", 49, 4, &pinctrl_port2 },    // P2.4
        { "J3", 58, 5, &pinctrl_port2 },    // P2.5
        { "J3", 50, 6, &pinctrl_port2 },    // P2.6
        { "J3", 45, 7, &pinctrl_port2 },    // P2.70
        { "J1", 49, 8, &pinctrl_port2 },    // P2.8
        { "J1", 50, 9, &pinctrl_port2 },    // P2.9
        { "J1", 51, 10, &pinctrl_port2 },   // P2.10
        { "J1", 52, 11, &pinctrl_port2 },   // P2.11
        { "J1", 53, 12, &pinctrl_port2 }    // P2.12
    }, 
    getDirection, 
    setDirection,
    getValue,
    setValue,
    setMux
} ;

struct Pinctrl pinctrl_port3 = {
    {
        0x40028000, // INP_STATE
        0x40028004, // OUTP_SET
        0x40028008, // OUTP_CLR
        0x4002800C, // OUTP_STATE
        0x0, // DIR_SET
        0x0, // DIR_CLR
        0x0 // DIR_STATE
    },
    number_pins_port3,
    {
        {"J3", 54, 25, &pinctrl_port3}, // P3.25
        {"J3", 46, 26, &pinctrl_port3}, // P3.26
        {"J3", 36, 29, &pinctrl_port3}, // P3.29
        {"J1", 24, 30, &pinctrl_port3}  // P3.30
    },
    getDirection,
    setDirection,
    getValue,
    setValue,
    NULL
};

#endif /*PINCTRL_LPC3250_H_*/
