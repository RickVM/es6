#include <linux/device.h> 
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <mach/hardware.h>

#define DEVICE_NAME "joystick"
#define CLASS_NAME "es6"

#define joystick_enable 101
#define joystick_read 102

#define P2_MUX_SET 0x40028028
#define P2_DIR_CLR 0x40028014
#define P2_INP_STATE 0x4002801C

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rick van Melis & Simon Lit");
MODULE_DESCRIPTION("Small module for reading the joystick on the LPC3250");
MODULE_VERSION("1.0");

static int majorNumber;
static int minor_num;
static int numberOpens = 0;
static struct class *joystickClass = NULL;   // Device-driver class struct pointer
static struct device *joystcikDevice = NULL; // Device-driver device struct pointer

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

static int __init joystick_init(void) {
  
  printk(KERN_INFO "JOYSTICK: Initializing the JOYSTICK\n");

  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  
  if (majorNumber < 0) {
    printk(KERN_ALERT "JOYSTICK: failed to register a major number\n");
  }

  printk(KERN_INFO "JOYSTICK: registered correctly with major number %d\n", majorNumber);

  // Register the device class
  joystickClass = class_create(THIS_MODULE, CLASS_NAME);
  
  if (IS_ERR(joystcikDevice
)) { // Check for error, cleanup if there is
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "JOYSTICK: failed to register device class\n");
    return PTR_ERR(joystickClass); // Correct way to return an error on a pointer
  }
  
  printk(KERN_INFO "JOYSTICK: device class registered correctly\n");

  joystcikDevice
 = device_create(joystickClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  
  if (IS_ERR(joystcikDevice
)) {
    class_destroy(joystickClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "JOYSTICK: failed to create the device\n");
    return PTR_ERR(joystcikDevice
  );
  }
  
  printk(KERN_INFO "JOYSTICK: device class created correctly\n");

  return 0;
}

static void __exit joystick_clean(void) {
  device_destroy(joystickClass, MKDEV(majorNumber, 0)); // remove the device
  class_unregister(joystickClass);                  // unregister the device class
  class_destroy(joystickClass);                     // remove the device class
  unregister_chrdev(majorNumber, DEVICE_NAME); // unregister the major number
  printk(KERN_INFO "JOYSTICK: Goodby from the LKM!\n");
}

// ---- OPEN & RELEASE ----

static int dev_open(struct inode *inodep, struct file *filep) {
  numberOpens++;
  printk(KERN_INFO "JOYSTICK: Device has been opened %d time(s)\n", numberOpens);
  minor_num = MINOR(inodep->i_rdev);
  printk(KERN_INFO "JOYSTICK: Opened for minor num: %i\n", minor_num);
  return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "JOYSTICK: Device is successfully closed\n");
  return 0;
}

// ---- READ METHODES ----

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
  int error_count = 0;
  int i = 0;
  unsigned long* memAddr = 0;
  const char* joystickMapping[5] = {"Click", "Left", "Up", "Right", "Down"};

  switch (minor_num) {
    case joystick_read:
      
      memAddr = io_p2v(P2_INP_STATE);

      printk("JOYSTICK: Value of register is: %lu\n", *memAddr);

      // Switch is a normal closed, so if it is zero we have an input
      for (i = 0; i < 5; i++) {
        if ((*memAddr & (1 << i)) == 0) {
          printk("JOYSTICK: %s!\n", joystickMapping[i]);
        }
      }

      break;
  }
  return error_count;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
  unsigned long* memAddr = 0;

  switch (minor_num) {
    case joystick_enable:
      memAddr = io_p2v(P2_MUX_SET);
      *memAddr |= ( 1 << 3);

      memAddr = io_p2v(P2_DIR_CLR);
      *memAddr |= 0x0000001F;

      printk(KERN_INFO "JOYSTICK: Enabled joystick!\n");

      break;
    default: 
      printk(KERN_INFO "JOYSTICK: Nothing to be writen to minor number!\n");
      break;
  }
  return len;
}


module_init(joystick_init) module_exit(joystick_clean)