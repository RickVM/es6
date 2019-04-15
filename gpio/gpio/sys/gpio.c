#include <linux/device.h>
#include <linux/kernel.h>  
#include <linux/kobject.h> 
#include <linux/module.h> 
#include <mach/hardware.h>
#include <linux/errno.h>

#define sysfs_dir "gpio"
#define sysfs_data "" // Maybe remove?
#define sysfs_max_data_size 1024

static char[] setPin;

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Melis & Lit");
MODULE_DESCRIPTION("sysfs GPIO");

static ssize_t config_read(struct device *dev, struct device_attribute *attr,
         char *buffer)
{
  printk(KERN_INFO "sysfile_read (/sys/kernel/%s/%s) called\n", sysfs_dir,
         attr->attr.name);
  // Use set pin to return state
  if(setPin !== NULL) {
    
  }
  else {
    printk("Read operation called without a set Connector + pin. Set a pin and try again.")
  }

  return 0; //RETURN SIZE OF BUFFER!!!!
}

// Writing does not use registerCount to enable variable size writing,
static ssize_t config_write(struct device *dev, struct device_attribute *attr,
          const char *buffer, size_t count)
{
  

  char connector[3];
  char direction[3];
  int pin;
  int result;

  printk(KERN_INFO "sysfile_write (/sys/kernel/%s/%s) called, buffer: "
      "%s, \n", sysfs_dir, attr->attr.name, buffer);

  // get connector, pin & direction from data. Save in global. (if not found, return error!)
  result = sscanf(buffer, "%s %d %s", connector , &pin, direction);
  if(result ==  3) { // OR result == 2? Only set connector+pin.
    printk(KERN_INFO "Connector: %s Pin: %d Direction: %s", connector, pin, direction);
    // Search Pin & connector in pinctrl
    // check if initialized
    // [no] -> initialize
    // Set direction
  }
  else {
    printk(KERN_WARNING "Input did not match expected format! connector pin direction e.g. j1 8 out");
    result = 1;
  }

  used_buffer_size = count > sysfs_max_data_size ? sysfs_max_data_size : count
  return used_buffer_size;
}

static DEVICE_ATTR(config, S_IWUGO | S_IRUGO, config_read, config_write);

static struct attribute *attrs[] = {
    &dev_attr_config.attr,
    NULL
};
static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *gpio_obj = NULL;

int __init sysfs_init(void)
{
  int result = 0;

  gpio_obj = kobject_create_and_add(sysfs_dir, kernel_kobj);
  if (gpio_obj == NULL) {
    printk(KERN_WARNING
        "%s module failed to load: kobject_create_and_add failed\n",
        sysfs_data);
    return -ENOMEM;
  }

  result = sysfs_create_group(gpio_obj, &attr_group);
  if (result != 0) {
    printk(KERN_WARNING "%s module failed to load: sysfs_create_group "
         "failed with result %d\n", sysfs_data, result);
    kobject_put(gpio_obj);
    return -ENOMEM;
  }

  printk(KERN_INFO "/sys/kernel/%s/%s created\n", sysfs_dir, sysfs_data);
  return result;
}

void __exit sysfs_exit(void)
{
  kobject_put(gpio_obj);
  printk(KERN_INFO "/sys/kernel/%s/%s removed\n", sysfs_dir, sysfs_data);
}

module_init(sysfs_init);
module_exit(sysfs_exit);