#include <linux/device.h>
#include <linux/kernel.h>  
#include <linux/kobject.h> 
#include <linux/module.h> 
#include <mach/hardware.h>
#include <linux/errno.h>

#include "../pinctrl_lpc3250.h"

#define sysfs_dir "gpio"
#define sysfs_max_data_size 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Melis & Lit");
MODULE_DESCRIPTION("sysfs GPIO");
MODULE_VERSION("1.0");

static struct Pin* activePin;

static ssize_t config_read(struct device *dev, struct device_attribute *attr,
         char *buffer)
{
  CONF direction;
  if(activePin == NULL) {
    printk(KERN_WARNING "Read operation called without a set Connector + pin."
             "Set a pin and try again.\n");
    return -ENXIO;
  }
  direction = activePin->pinctrl->get_direction(activePin);
  return snprintf(buffer, sysfs_max_data_size, "%s %d %c", 
                    activePin->connector, activePin->pin, conf_to_char(direction));
}

static ssize_t config_write(struct device *dev, struct device_attribute *attr,
          const char *buffer, size_t count)
{
  char connector[3];
  char direction;
  int pinNr;
  int result;
  int used_buffer_size;

  // get connector, pin & direction from data. Save in global. (if not found, return error!)
  result = sscanf(buffer, "%s %d %c", connector , &pinNr, &direction);
  if(result ==  3) { 
      printk(KERN_INFO "Received cmd: Connector: %s Pin: %d Direction: %c\n", connector, pinNr, direction);
      activePin = (struct Pin*)searchPin(connector, pinNr);
      if(activePin != NULL) { 
        if (activePin->pinctrl->init != NULL) {
          printk(KERN_INFO "Executing init");
          if (activePin->pinctrl->init() != 0) {
            printk(KERN_WARNING "Could not initialize Connector: %s Pin: %d perhaps it is used by another device?", connector, pinNr);
            return count > sysfs_max_data_size ? sysfs_max_data_size : count;
          }
        }
        if(activePin->pinctrl->set_direction(activePin, direction) != 0) {
          printk(KERN_WARNING "Could not set pin direction for Connector: %s Pin: %d Direction: %c\n", connector, pinNr, direction);
       } 
    } else {
      printk(KERN_WARNING "Could not find match for Connector: %s Pin: %d\n", connector, pinNr);
   }
  } else {
    printk(KERN_WARNING "Input did not match expected format! Usage: connector pin direction e.g. J1 8 O\n");
  }
  
  used_buffer_size = count > sysfs_max_data_size ? sysfs_max_data_size : count;
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
        "module failed to load: kobject_create_and_add failed\n");
    return -ENOMEM;
  }

  result = sysfs_create_group(gpio_obj, &attr_group);
  if (result != 0) {
    printk(KERN_WARNING "module failed to load: sysfs_create_group "
         "failed with result %d\n", result);
    kobject_put(gpio_obj);
    return -ENOMEM;
  }

  printk(KERN_INFO "/sys/kernel/%s/ created\n", sysfs_dir);
  return result;
}

void __exit sysfs_exit(void)
{
  kobject_put(gpio_obj);
  printk(KERN_INFO "/sys/kernel/%sremoved\n", sysfs_dir);
}

module_init(sysfs_init);
module_exit(sysfs_exit);