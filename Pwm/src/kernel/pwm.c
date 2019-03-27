#include <linux/device.h> 
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h> 

#define DEVICE_NAME "pwm"
#define CLASS_NAME "es6"

#define pwm1_enable 11
#define pwm1_freq 12
#define pwm1_duty 13
#define pwm2_duty 23
#define pwm2_enable 21
#define pwm2_freq 22

#define PWM1_CTRL 0x4005c000 
#define PWM2_CTRL 0x4005C004
// Load with mknod type MAJOR MINOR

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rick van Melis & Simon Lit");
MODULE_DESCRIPTION("PWM module for the LPC3250");
MODULE_VERSION("1.0");

static int majorNumber;
static int minor_num;
static int numberOpens = 0;
static struct class *pwmClass = NULL;   // Device-driver class struct pointer
static struct device *pwmDevice = NULL; // Device-driver device struct pointer

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

static int __init pwm_init(void) {
  printk(KERN_INFO "PWM: Initializing the PWM\n");

  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  
  if (majorNumber < 0) {
    printk(KERN_ALERT "PWM: failed to register a major number\n");
  }

  printk(KERN_INFO "PWM: registered correctly with major number %d\n", majorNumber);

  // Register the device class
  pwmClass = class_create(THIS_MODULE, CLASS_NAME);
  
  if (IS_ERR(pwmDevice)) { // Check for error, cleanup if there is
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "PWM: failed to register device class\n");
    return PTR_ERR(pwmClass); // Correct way to return an error on a pointer
  }
  
  printk(KERN_INFO "PWM: device class registered correctly\n");

  pwmDevice = device_create(pwmClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  
  if (IS_ERR(pwmDevice)) {
    class_destroy(pwmClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "PWM: failed to create the device\n");
    return PTR_ERR(pwmDevice);
  }
  
  printk(KERN_INFO "PWM: device class created correctly\n");

  return 0;
}

static void __exit pwm_clean(void) {
  device_destroy(pwmClass, MKDEV(majorNumber, 0)); // remove the device
  class_unregister(pwmClass);                  // unregister the device class
  class_destroy(pwmClass);                     // remove the device class
  unregister_chrdev(majorNumber, DEVICE_NAME); // unregister the major number
  printk(KERN_INFO "PWM: Goodby from the LKM!\n");
}

// This function is called each time to the device is opened, only increments
// the numberOpends counter.
static int dev_open(struct inode *inodep, struct file *filep) {
  numberOpens++;
  printk(KERN_INFO "PWM: Device has been opened %d time(s)\n", numberOpens);

  // Todo: Use file to set private data field with minor num.
  minor_num = MINOR(inodep->i_rdev);
  printk(KERN_INFO "PWM: Opened for minor num: %i", minor_num);
  return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
  int error_count = 0;
  printk(KERN_INFO "PWM: Device has been read for minor num: %i", minor_num);
  
  switch (minor_num) {
    case pwm1_enable:

      break;

    case pwm1_freq:

      break;

    case pwm1_duty:

      break;
    case pwm2_enable:

      break;

    case pwm2_freq:

      break;

    case pwm2_duty:

      break;
  }
  // error_count = copy_to_user(buffer, message, size_of_message);
  return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len,
                         loff_t *offset) {
  printk(KERN_INFO "PWM: Device has been written to for minor num: %i", minor_num);
  //    char temp[len];
  //    memset(temp, 0, sizeof temp);
  //    copy_from_user(temp, buffer, (unsigned long) len);
  switch (minor_num) {
    case pwm1_enable:

      break;

    case pwm1_freq:

      break;

    case pwm1_duty:

      break;
    case pwm2_enable:

      break;

    case pwm2_freq:

      break;

    case pwm2_duty:

      break;
  }
  return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "PWM: Device is successfully closed");
  return 0;
}

module_init(pwm_init) module_exit(pwm_clean)