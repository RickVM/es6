/*
 * sysfs3.c - create a "subdir" with multiple "files" in /sys
 *
 */
#include <linux/device.h>
#include <linux/kernel.h>  /* We're doing kernel work */
#include <linux/kobject.h> /* Necessary because we use sysfs */
#include <linux/module.h>  /* Specifically, a module */
#include <mach/hardware.h>


#define sysfs_dir "peek"
#define sysfs_data "data"
/* due to limitations of sysfs, you mustn't go above PAGE_SIZE, 1k is already a
 * *lot* of information for sysfs! */
#define sysfs_max_data_size 1024

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Melis & Lit");
MODULE_DESCRIPTION("sysfs poker");

static char sysfs_buffer[sysfs_max_data_size + 1] =
    "Placeholder data"; /* an extra byte for the '\0' terminator */
static ssize_t used_buffer_size = 0;
static unsigned long* memAddr;
static unsigned long registerCount = 0;
static ssize_t data_read(struct device *dev, struct device_attribute *attr,
         char *buffer)
{
  printk(KERN_INFO "sysfile_read (/sys/kernel/%s/%s) called\n", sysfs_dir,
         attr->attr.name);
  
  /*
   * The only change here is that we now increment nr_buffer_reads (and
   * don't worry about overflows - which you should in a real driver)
   */
  return sprintf(buffer, "%lu", *memAddr);
}

static ssize_t data_write(struct device *dev, struct device_attribute *attr,
          const char *buffer, size_t count)
{
  used_buffer_size =
      count > sysfs_max_data_size
    ? sysfs_max_data_size
    : count; /* handle MIN(used_buffer_size, count) bytes */

  printk(KERN_INFO "sysfile_write (/sys/kernel/%s/%s) called, buffer: "
       "%s, count: %u\n", sysfs_dir, sysfs_data, buffer, count);

  memcpy(sysfs_buffer, buffer, used_buffer_size);
  sysfs_buffer[used_buffer_size] = '\0'; /* this is correct, the buffer
      is declared to be sysfs_max_data_size+1 bytes! */

  return used_buffer_size;
}

static ssize_t address_read(struct device *dev, struct device_attribute *attr,
       char *buffer)
{
  printk(KERN_INFO "address read (/sys/kernel/%s/%s) called\n", sysfs_dir,
         attr->attr.name);

  return sprintf(buffer, "0x%lx", io_v2p((unsigned long)memAddr));
}

static ssize_t address_write(struct device *dev, struct device_attribute *attr,
        const char *buffer, size_t count)
{
  unsigned long cast;
  printk("Address write called. value: %s\n", buffer);
  if(buffer == NULL) {
    printk(KERN_WARNING "Empty input buffer for memAddr");
    return -EINVAL;
  }
  if(count > 9) {
    printk(KERN_WARNING "Input longer than 8");
    count = 9;
  }
  strict_strtoul(buffer, 16, &cast);
  printk("Saved addres: 0x%lx", cast);
  
  memAddr = io_p2v(cast);
  return count;
}

static ssize_t count_read(struct device *dev, struct device_attribute *attr,
       char *buffer)
{
  printk(KERN_INFO "address read (/sys/kernel/%s/%s) called\n", sysfs_dir,
         attr->attr.name);

  return sprintf(buffer, "%lu", registerCount);
}

static ssize_t count_write(struct device *dev, struct device_attribute *attr,
        const char *buffer, size_t count)
{
  printk("count write called. value: %s\n", buffer);
  if(buffer == NULL) {
    printk(KERN_WARNING "Empty input buffer for memAddr");
    return -EINVAL;
  }

  strict_strtoul(buffer, 10, &registerCount);
  printk("Saved count: %lu", registerCount);
  
  return count;
}

static DEVICE_ATTR(data, S_IWUGO | S_IRUGO, data_read, data_write);
static DEVICE_ATTR(address, S_IWUGO | S_IRUGO, address_read, address_write);
static DEVICE_ATTR(count, S_IWUGO | S_IRUGO, count_read, count_write);
/*
 * The only changes here are the added files to the file list:
 */
static struct attribute *attrs[] = {
    &dev_attr_data.attr,
    &dev_attr_address.attr,
    &dev_attr_count.attr,
    NULL /* need to NULL terminate the list of attributes */
};
static struct attribute_group attr_group = {
    .attrs = attrs,
};
static struct kobject *poke_obj = NULL;

int __init sysfs_init(void)
{
  int result = 0;

  /*
   * This is identical to previous example.
   */
  poke_obj = kobject_create_and_add(sysfs_dir, kernel_kobj);
  if (poke_obj == NULL) {
    printk(KERN_WARNING
        "%s module failed to load: kobject_create_and_add failed\n",
        sysfs_data);
    return -ENOMEM;
  }

  result = sysfs_create_group(poke_obj, &attr_group);
  if (result != 0) {
    /* creating files failed, thus we must remove the created
     * directory! */
    printk(KERN_WARNING "%s module failed to load: sysfs_create_group "
         "failed with result %d\n", sysfs_data, result);
    kobject_put(poke_obj);
    return -ENOMEM;
  }

  printk(KERN_INFO "/sys/kernel/%s/%s created\n", sysfs_dir, sysfs_data);
  return result;
}

void __exit sysfs_exit(void)
{
  kobject_put(poke_obj);
  printk(KERN_INFO "/sys/kernel/%s/%s removed\n", sysfs_dir, sysfs_data);
}

module_init(sysfs_init);
module_exit(sysfs_exit);