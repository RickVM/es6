#include <linux/device.h> 
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <mach/hardware.h>

#include "../pinctrl_lpc3250.h"

#define DEVICE_NAME "gpio"
#define CLASS_NAME "es6"

#define gpio_set 101
#define gpio_read 102

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rick van Melis & Simon Lit");
MODULE_DESCRIPTION("Module for reading and writing to GPIO's on the LPC3250");
MODULE_VERSION("1.0");

static int majorNumber;
static int minor_num;
static int numberOpens = 0;
static struct class *gpioClass = NULL;   // Device-driver class struct pointer
static struct device *gpioDevice = NULL; // Device-driver device struct pointer

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
  .open = dev_open,
  .read = dev_read,
  .write = dev_write,
  .release = dev_release
  };

// ---- INIT & EXIT ----

static int __init gpio_init(void) {
  
  printk(KERN_INFO "GPIO: Initializing the GPIO\n");

  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  
  if (majorNumber < 0) {
    printk(KERN_ALERT "GPIO: failed to register a major number\n");
  }

  printk(KERN_INFO "GPIO: registered correctly with major number %d\n", majorNumber);

  // Register the device class
  gpioClass = class_create(THIS_MODULE, CLASS_NAME);
  
  if (IS_ERR(gpioDevice)) { // Check for error, cleanup if there is
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "GPIO: failed to register device class\n");
    return PTR_ERR(gpioClass); // Correct way to return an error on a pointer
  }
  
  printk(KERN_INFO "GPIO: device class registered correctly\n");

  gpioDevice = device_create(gpioClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  
  if (IS_ERR(gpioDevice)) {
    class_destroy(gpioClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "GPIO: failed to create the device\n");
    return PTR_ERR(gpioDevice);
  }
  
  printk(KERN_INFO "GPIO: device class created correctly\n");
  
  return 0;
}

static void __exit gpio_exit(void) {
  device_destroy(gpioClass, MKDEV(majorNumber, 0)); // remove the device
  class_unregister(gpioClass);                  // unregister the device class
  class_destroy(gpioClass);                     // remove the device class
  unregister_chrdev(majorNumber, DEVICE_NAME); // unregister the major number
  printk(KERN_INFO "GPIO: Goodby from the LKM!\n");
}

// ---- OPEN & RELEASE ----

static int dev_open(struct inode *inodep, struct file *filep) {
  numberOpens++;
  printk(KERN_INFO "GPIO: Device has been opened %d time(s)\n", numberOpens);
  minor_num = MINOR(inodep->i_rdev);
  printk(KERN_INFO "GPIO: Opened for minor num: %i\n", minor_num);
  return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "GPIO: Device is successfully closed\n");
  return 0;
}

// ---- READ METHODES ----

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
  int error_count = 0;
  uint8_t retv = -1;
  unsigned long* memAddr = 0;

  switch (minor_num) {
    case gpio_set:
    
    memAddr = io_p2v(pinctrl_port2.registers.INP_STATE);

    printk(KERN_INFO "GPIO: Memory adress is: %lu\n", pinctrl_port2.registers.INP_STATE);
    printk(KERN_INFO "GPIO: Number of pins is: %d\n", pinctrl_port2.npins);
    printk(KERN_INFO "GPIO: Return value through struct: %d\n", pinctrl_port2.get_value(0x40024008, 10));

    //   memAddr = io_p2v(P2_INP_STATE);

    //   printk("GPIO: Value of register is: %lu\n", *memAddr);

    //   // Switch is a normal closed, so if it is zero we have an input
    //   for (i = 0; i < 5; i++) {
    //     if ((*memAddr & (1 << i)) == 0) {
    //       printk("GPIO: %s!\n", GPIOMapping[i]);
    //       retv = i;
    //     }
    //   }

      break;
    
    case gpio_read: 

        break;
  }
  error_count = copy_to_user(buffer, &retv, sizeof retv);
  return error_count;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
  unsigned long* memAddr = 0;

  switch (minor_num) {
    case gpio_set:
     

    memAddr = io_p2v(pinctrl_port2.registers.INP_STATE);

    //   memAddr = io_p2v(P2_MUX_SET);
    //   *memAddr |= ( 1 << 3);

    //   memAddr = io_p2v(P2_DIR_CLR);
    //   *memAddr |= 0x0000001F;


      break;

    case gpio_read:

      break;

    default: 
      printk(KERN_INFO "GPIO: Nothing to be writen to minor number!\n");
      break;
  }
  return len;
}


module_init(gpio_init) module_exit(gpio_exit)