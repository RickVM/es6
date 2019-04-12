#ifndef PINCTRL_H
#define PINCTRL_H

#define PINS_PER_CONNECTOR 65

struct Pinctrl;
struct Pin;
struct Registers;

typedef enum {
    disabled = 0,
    output = 1,
    input = 2
} CONF;

typedef CONF (*Get_direction)(struct Registers* registers, int index);
typedef void (*Set_direction)(struct Registers* registers, int index);
typedef int (*Get_value)(unsigned long reg, int index);
typedef void (*Set_value)(struct Registers* reg, int index);
typedef int (*Init)(void);

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
    struct Pinctrl *pinctrl; // pointer to parent
};

struct Pinctrl {
    struct Registers registers;
    unsigned int npins;
    struct Pin pins[PINS_PER_CONNECTOR];
    Get_direction get_direction;
    Set_direction set_direction;
    Get_value get_value;
    Set_value set_value;
    Init init;
};

#endif /* PINCTRL_H */