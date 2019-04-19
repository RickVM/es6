#include "pinctrl_lpc3250.h"

#define number_pins_port0 8
#define number_pins_port2 13
#define number_pins_port3 4

#define number_of_pin_controllers 3

#define P2_MUX_SET 0x40028028
#define EMC_D_SEL 3
#define LCD_CTRL 0x31040018
#define LCD_CTRL_VALUE 0x0

static const struct Pinctrl *pinctrls[number_of_pin_controllers] = { &pinctrl_port0, &pinctrl_port2, &pinctrl_port3};

char conf_to_char(CONF direction) {
  char dir;
  switch(direction) {
    case output:
      dir = 'O';
      break;
    case input:
      dir = 'I';
      break;
    case disabled:
      dir = 'D';
      break;
    default:
      dir = 'E';
      printk(KERN_ERR "No matching direction for %d\n", direction);
      break;
  };
  return dir;
}

const struct Pin* searchPin(const char* connector, int number) {
    uint8_t k = 0;
    uint8_t i = 0;

    for (k=0; k< number_of_pin_controllers; k++) {
        for (i=0; i<pinctrls[k]->npins; i++) {
            if (strcmp(pinctrls[k]->pins[i].connector, connector) == 0 && pinctrls[k]->pins[i].pin == number) {
               return &pinctrls[k]->pins[i];
            } 
        }
    }

    return NULL;
}

CONF getDirection(struct Pin* pin) {
    unsigned long* memAddr = 0;
    CONF conf = disabled;
    uint32_t dir = -1;

    memAddr = io_p2v(pin->pinctrl->registers.DIR_STATE);
    dir = *memAddr & ( 1UL << pin->index_write );
    conf = (CONF) dir >> pin->index_write;

    return conf;
}

int setDirection(struct Pin* pin, const char direction) {
    int retv = 0;
    unsigned long* memAddr = 0;
    printk(KERN_INFO "Setting direction for: Connector: %s Pin: %d Direction: %c\n", pin->connector, pin->pin, direction);

    switch (direction) {
        case 'O':
        case 'o':
            memAddr = io_p2v(pin->pinctrl->registers.DIR_SET);
            *memAddr |= ( 1UL << pin->index_write );
            break;
        case 'I':
        case 'i':
            memAddr = io_p2v(pin->pinctrl->registers.DIR_CLR);
            *memAddr |= ( 1UL << pin->index_write );
            break;
        default:
            retv = -1;
            break;
    }
    return retv;
}

int getValue(struct Pin* pin) {
    uint32_t value = 0;
    unsigned long* memAddr = 0;

    CONF dir = getDirection(pin);

    switch ( dir )
    {
        case input: // Pin is configured as input
            memAddr = io_p2v(pin->pinctrl->registers.INP_STATE);
            value = *memAddr & ( 1UL << pin->index_read );
            break;
        case output: // Pin is configured as output
            memAddr = io_p2v(pin->pinctrl->registers.OUTP_STATE);
            value =  *memAddr & ( 1UL << pin->index_read );
            break;

        default:
            value = -1;
            break;
    }
    return value >> pin->index_write;
}

int setValue(struct Pin* pin, int value) {
    unsigned long* memAddr = 0;
    uint8_t retv = 0;
    
    switch (value)
    {
        case 0:
            memAddr = io_p2v(pin->pinctrl->registers.OUTP_CLR);
            *memAddr |= ( 1UL << pin->index_write );
            break;
        case 1:
            memAddr = io_p2v(pin->pinctrl->registers.OUTP_SET);
            *memAddr |= ( 1UL << pin->index_write );
            break;
        default:
            retv = -1;
            break;
    }
    return retv;
}

int setMux(void) {
    unsigned long* memAddr = 0;
    printk(KERN_INFO "Setting Mux");
    memAddr = io_p2v(P2_MUX_SET);
    *memAddr |= (1UL << EMC_D_SEL);
    return 0;
}

int turnOffLcd(void) {
    unsigned long* memAddr = 0;
    printk(KERN_INFO "Turning off LCD");
    memAddr = io_p2v(LCD_CTRL);
    *memAddr = LCD_CTRL_VALUE;
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
        {"J3", 40, 0, 0, &pinctrl_port0},  // P0.0
        {"J2", 24, 1, 1, &pinctrl_port0},  // P0.1
        {"J2", 11, 2, 2, &pinctrl_port0},  // P0.2
        {"J2", 12, 3, 3, &pinctrl_port0},  // P0.3
        {"J2", 13, 4, 4, &pinctrl_port0},  // P0.4
        {"J2", 14, 5, 5, &pinctrl_port0},  // P0.5
        {"J3", 33, 6, 6, &pinctrl_port0},  // P0.6
        {"J1", 27, 7, 7, &pinctrl_port0},  // P0.7
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
        { "J3", 47, 0, 0, &pinctrl_port2 },    // P2.0
        { "J3", 56, 1, 1, &pinctrl_port2 },    // P2.1
        { "J3", 48, 2, 2, &pinctrl_port2 },    // P2.2
        { "J3", 57, 3, 3, &pinctrl_port2 },    // P2.3
        { "J3", 49, 4, 4, &pinctrl_port2 },    // P2.4
        { "J3", 58, 5, 5, &pinctrl_port2 },    // P2.5
        { "J3", 50, 6, 6, &pinctrl_port2 },    // P2.6
        { "J3", 45, 7, 7, &pinctrl_port2 },    // P2.70
        { "J1", 49, 8, 8, &pinctrl_port2 },    // P2.8
        { "J1", 50, 9, 9, &pinctrl_port2 },    // P2.9
        { "J1", 51, 10, 10, &pinctrl_port2 },   // P2.10
        { "J1", 52, 11, 11, &pinctrl_port2 },   // P2.11
        { "J1", 53, 12, 12, &pinctrl_port2 }    // P2.12
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
        0x40028010, // P2 DIR_SET
        0x40028014, // P2 DIR_CLR
        0x40028018  // P2 DIR_STATE
    },
    number_pins_port3,
    {
        {"J3", 54, 10, 25, &pinctrl_port3}, // P3.25
        {"J3", 46, 11, 26, &pinctrl_port3}, // P3.26
        {"J3", 36, 14, 29, &pinctrl_port3}, // P3.29
        {"J1", 24, 24, 30, &pinctrl_port3}  // P3.30
    },
    getDirection,
    setDirection,
    getValue,
    setValue,
    NULL
};
