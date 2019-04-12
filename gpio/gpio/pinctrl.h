#ifndef PINCTRL_H
#define PINCTRL_H

struct Pinctrl;
struct Pin;
struct Registers;

enum CONF;

typedef CONF (*Get_direction)(Registers registers, int index);
typedef void (*Set_dir)(Registers registers, int index);
typedef int (*Get_value)(unsigned long reg, int index);
typedef void (*Set_value)(Registers reg, int index);
typedef int (*Init)();

enum CONF {
    disabled,
    output,
    input
};

struct Registers {
    unsigned long INP_STATE; // Read state of input pins 
    unsigned long OUTP_SET; // Set output pins to high
    unsigned long OUTP_CLR; // Set output pins to low
    unsigned long OUTP_STATE; // Read the state of the output pin
    unsigned long DIR_SET; // Set the direction of the pin as output
    unsigned long DIR_CLR; // Set the direction of the pin as input
    unsigned long DIR_STATE; // Read the state of the direction
};

struct Pin {
    const char* connector;
    unsigned int pin;
    unsigned int index;
    Pinctrl pinctrl; // refrence to parent
};

struct Pinctrl {
    Registers registers;
    unsigned int npins;
};

#endif /* PINCTRL_H */