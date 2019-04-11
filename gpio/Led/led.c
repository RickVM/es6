#include <linux/device.h> 
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <mach/hardware.h>

#define DEVICE_NAME "led"
#define CLASS_NAME "es6"

#define enable_leds 1
#define select_led 2
#define set_led 3

#define P2_MUX_SET 0x40028028
#define P2_DIR_SET 0x40028010
#define P2_OUTP_SET 0x40028020
#define P2_OUTP_CLR 0x40028024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rick van Melis & Simon Lit");
MODULE_DESCRIPTION("Small module for setting the leds on the LPC3250");
MODULE_VERSION("1.0");

static int majorNumber;
static int minor_num;
static int numberOpens = 0;
static struct class *ledClass = NULL;   // Device-driver class struct pointer
static struct device *ledDevice = NULL; // Device-driver device struct pointer
static int selectedLed = 0;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
  .open = dev_open,
  .write = dev_write,
  .release = dev_release
  };

// ---- INIT & EXIT ----

static int __init LED_init(void) {
  
  printk(KERN_INFO "LED: Initializing the LED\n");

  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  
  if (majorNumber < 0) {
    printk(KERN_ALERT "LED: failed to register a major number\n");
  }

  printk(KERN_INFO "LED: registered correctly with major number %d\n", majorNumber);

  // Register the device class
  ledClass = class_create(THIS_MODULE, CLASS_NAME);
  
  if (IS_ERR(ledDevice
)) { // Check for error, cleanup if there is
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "LED: failed to register device class\n");
    return PTR_ERR(ledClass); // Correct way to return an error on a pointer
  }
  
  printk(KERN_INFO "LED: device class registered correctly\n");

  ledDevice
 = device_create(ledClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  
  if (IS_ERR(ledDevice
)) {
    class_destroy(ledClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "LED: failed to create the device\n");
    return PTR_ERR(ledDevice
  );
  }
  
  printk(KERN_INFO "LED: device class created correctly\n");

  return 0;
}

static void __exit LED_clean(void) {
  device_destroy(ledClass, MKDEV(majorNumber, 0)); // remove the device
  class_unregister(ledClass);                  // unregister the device class
  class_destroy(ledClass);                     // remove the device class
  unregister_chrdev(majorNumber, DEVICE_NAME); // unregister the major number
  printk(KERN_INFO "LED: Goodby from the LKM!\n");
}

// ---- OPEN & RELEASE ----

static int dev_open(struct inode *inodep, struct file *filep) {
  numberOpens++;
  printk(KERN_INFO "LED: Device has been opened %d time(s)\n", numberOpens);
  // Todo: Use file to set private data field with minor num.
  minor_num = MINOR(inodep->i_rdev);
  printk(KERN_INFO "LED: Opened for minor num: %i", minor_num);
  return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "LED: Device is successfully closed\n");
  return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
  int retv = 0;
  char _buffer[len];
  unsigned long int res = 0;
  char *ptr;
  unsigned long* memAddr = 0;

  printk(KERN_INFO "PWM: Device has been written to for minor num: %i", minor_num);
  
  memset(_buffer, 0, sizeof _buffer);
  retv = copy_from_user(_buffer, buffer, (unsigned long) len);   

  res = simple_strtoul(_buffer, &ptr, 10);
  
  printk(KERN_INFO "LED: Device has been written to for minor num: %i", minor_num);

  switch (minor_num) {
    case enable_leds:
      // Enable LED
      memAddr = io_p2v(P2_MUX_SET);
      *memAddr |= ( 1 << 3);

      memAddr = io_p2v(P2_DIR_SET);
      *memAddr |= 0x00001C00;

      printk(KERN_INFO "LED: Enabled LED!\n");

      break;

    case select_led:
      if (res > 0 && res < 4 ) {
        switch (res)
        {
          case 1:
            selectedLed = 10;
            break;
          case 2:
            selectedLed = 11;
            break;
          case 3: 
            selectedLed = 12;
            break;
          default:
            break;
        }

        printk(KERN_INFO "LED: Led %lu has been selected.\n", res);
      }
      break;

    case set_led:
      switch(res) {
        case 0: 
          memAddr = io_p2v(P2_OUTP_CLR);
          *memAddr |= ( 1 << selectedLed);
          printk(KERN_INFO "LED: Led %d has been turned off\n", selectedLed);
          break;
        case 1: 
          memAddr = io_p2v(P2_OUTP_SET);
          *memAddr |= ( 1 << selectedLed);
          printk(KERN_INFO "LED: Led %d has been turned on\n", selectedLed);
          break;
        default:
          printk(KERN_INFO "LED: Invalid input!\n");
          break;
      }
      break;
  }
  return len;
}


module_init(LED_init) module_exit(LED_clean)