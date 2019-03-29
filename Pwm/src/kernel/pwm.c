#include <linux/device.h> 
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <mach/hardware.h>

#define DEVICE_NAME "pwm"
#define CLASS_NAME "es6"

#define pwm1_enable 11
#define pwm1_freq 12
#define pwm1_duty 13
#define pwm2_enable 21
#define pwm2_freq 22
#define pwm2_duty 23

#define PWM1_CTRL 0x4005c000  
#define PWM2_CTRL 0x4005C004
#define PWM_ENABLE_BIT 30
#define PWM_DUTY 0xFFFFFF00
#define PWM_FREQ 0xFFFF00FF
#define PWM_FREQ_OFFSET 8

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

// ---- INIT & EXIT ----

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

// ---- OPEN & RELEASE ----

static int dev_open(struct inode *inodep, struct file *filep) {
  numberOpens++;
  printk(KERN_INFO "PWM: Device has been opened %d time(s)\n", numberOpens);
  // Todo: Use file to set private data field with minor num.
  minor_num = MINOR(inodep->i_rdev);
  printk(KERN_INFO "PWM: Opened for minor num: %i", minor_num);
  return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "PWM: Device is successfully closed\n");
  return 0;
}

// ---- READ METHODES ----

uint8_t readEnable (unsigned long address) {
  unsigned long* memAddr = NULL;
  uint8_t value = 0;

  memAddr = io_p2v(address);
  printk(KERN_INFO "PWM: Decimal value of memAddress is: %lu", *memAddr);
  
  value = (*memAddr & (1 << PWM_ENABLE_BIT)) >> PWM_ENABLE_BIT;
  printk(KERN_INFO "PWM: Enable value is %d", value);

  return value; 
}

uint8_t readFreq (unsigned long address) {
  unsigned long* memAddr = NULL;
  u_int8_t value = 0;

  memAddr = io_p2v(address);
  printk(KERN_INFO "PWM: Decimal value of memAddress is: %lu", *memAddr);

  value = (*memAddr & (*memAddr | PWM_FREQ)) >> PWM_FREQ_OFFSET;
  printk(KERN_INFO "PWM: Freq value is %d", value);

  return value;
}

uint8_t readDuty (unsigned long address) {
  unsigned long* memAddr = NULL;
  u_int8_t value = 0;

  memAddr = io_p2v(address);
  printk(KERN_INFO "PWM: Decimal value of memAddress is: %lu", *memAddr);

  value = *memAddr & (*memAddr | PWM_DUTY);
  printk(KERN_INFO "PWM: Duty value is %d", value);

  return value;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
  int error_count = 0;
  uint8_t value = 0;
  printk(KERN_INFO "PWM: Device has been read for minor num: %i", minor_num);
  switch (minor_num) {
    case pwm1_enable:
      value = readEnable(PWM1_CTRL);
      break;
    case pwm1_freq:
      value = readFreq(PWM1_CTRL);
      break;
    case pwm1_duty:
      value = readDuty(PWM1_CTRL);
      break;
    case pwm2_enable:
      value = readEnable(PWM2_CTRL);
      break;
    case pwm2_freq:
      value = readFreq(PWM2_CTRL);
      break;
    case pwm2_duty:
      value = readDuty(PWM2_CTRL);
      break;
  }
  error_count = copy_to_user(buffer, &value, sizeof value);
  return error_count;
}

// ---- WRITE METHODES ----

// Set bit 31 of the register with the value passed
int writeEnable(unsigned long address, uint8_t value) {
  unsigned long* memAddr = 0;

  if (value != 0 && value != 1) {
    printk(KERN_INFO "PWM: Enable, incorrect input. Should be 0 or 1.");
    return -1; // TODO Return correct error
  }

  memAddr = io_p2v(address);

  switch (value) {
    case 0: 
      *memAddr = *memAddr & ~(1 << PWM_ENABLE_BIT); 
      break;
    case 1:
      *memAddr = *memAddr | (1 << PWM_ENABLE_BIT); 
      break;
  }

  return 0;
}

// Write to bits 15:8 for changing the output frequency
int writeFreq(unsigned long address, uint8_t value) {
  unsigned long* memAddr = 0;

  printk(KERN_INFO "PWM: Freq, writing a freq of %d", value);

  memAddr = io_p2v(address);
  *memAddr = (*memAddr & PWM_FREQ) | (value << PWM_FREQ_OFFSET);

  return 0;
}

// Wite to bits 7:0 for adjusting the duty cycle
int writeDuty(unsigned long address, uint8_t value) {
  unsigned long* memAddr = 0;

  printk(KERN_INFO "PWM: Duty, writing a duty of %d", value);

  memAddr = io_p2v(address);
  *memAddr = (*memAddr & PWM_DUTY) | value;

  return 0;
}


static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
  int retv = 0;
  char _buffer[len];
  uint8_t value = 0;
  unsigned long int res = 0;
  char *ptr;

  printk(KERN_INFO "PWM: Device has been written to for minor num: %i", minor_num);
  
  memset(_buffer, 0, sizeof _buffer);
  retv = copy_from_user(_buffer, buffer, (unsigned long) len);   
  
  // TODO Check return value
  // TODO Check for negative numbers, they are now coverted to a zero. Not exactly what we want
  
  res = simple_strtoul(_buffer, &ptr, 10);
  printk(KERN_INFO "PWM: Number is %lu", res);
  
  if (res > 255 || res < 0) {
    printk(KERN_INFO "PWM: Input for writing to device is invalid. Not a uint8_t.");
    return -1; // TODO Return the correct error here
  }
  
  value = (uint8_t) res;
  
  printk(KERN_INFO "PWM: Input value is: %i", value);

  switch (minor_num) {
    case pwm1_enable:
      if (writeEnable(PWM1_CTRL ,value)) {
        // TODO Check return value
      }
      break;
    case pwm1_freq:
      if (writeFreq(PWM1_CTRL, value)) {
        // TODO Check return value
      }
      break;
    case pwm1_duty:
      // TODO Be able to enter a duty cycle of 100 % instead of 255
      // if (value > 100) {
      //   printk(KERN_INFO "PWM: Duty, incorrect input. Should be between 0 and 100 procent.");
      //   return -1; // TODO Return correct error
      // }
      if (writeDuty(PWM1_CTRL, value)) {
        // TODO Check return value
      }
      break;
    case pwm2_enable:
      if (writeEnable(PWM2_CTRL ,value)) {
        // TODO Check return value
      }
      break;
    case pwm2_freq:
      if (writeFreq(PWM2_CTRL, value)) {
        // TODO Check return value
      }
      break;
    case pwm2_duty:
      if (writeDuty(PWM2_CTRL, value)) {
        // TODO Check return value
      }
      break;
  }
  return len;
}


module_init(pwm_init) module_exit(pwm_clean)