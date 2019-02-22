#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>   // Header support the kernel Driver model
#include <linux/fs.h>       // Header for the Linux file system support
#include <linux/uaccess.h>   // Header for the copy to user function

#define DEVICE_NAME "peek-poke"
#define CLASS_NAME "pp"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Simon Lit");
MODULE_DESCRIPTION("A simple Linux char driver for the LPC3250");
MODULE_VERSION("0.1");

static int majorNumber;
static int numberOpens = 0;
static struct class* ppClass = NULL;   // Device-driver class struct pointer
static struct device* ppDevice = NULL; // Device-driver device struct pointer

// The prototype functions for the character driver -- must come before the struct defintion
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/*
 * The file_operations structure from /linux/fs.h lists the callback functions that you wish to associated with your file operations
 * using a C99 syntax structure. char devices usually implement open, read, write and release calls
*/

static struct file_operations fops = 
{ 
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release
};

static int __init ebb_init(void) {
    printk(KERN_INFO "PEEK-POKE: Initializing the PEEK-POKE\n");

    // Try to dynamically allocate major number for the device
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "PEEK-POKE: failed to register a major number\n");
    }
    printk(KERN_INFO "PEEK-POKE: registered correctly with major number %d\n", majorNumber);

    // Register the device class 
    ppClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(ppDevice)) { // Check for error, cleanup if there is
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "PEEK-POKE: failed to register device class\n");
        return PTR_ERR(ppClass); // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "PEEK-POKE: device class registered correctly\n");

    ppDevice = device_create(ppClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(ppDevice)) {
        class_destroy(ppClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "PEEK-POKE: failed to create the device\n");
        return PTR_ERR(ppDevice);
    }
    printk(KERN_INFO "PEEK-POKE: device class created correctly\n");

    return 0;
}

static void __exit ebb_clean(void) {
    device_destroy(ppClass, MKDEV(majorNumber, 0));     // remove the device
    class_unregister(ppClass);                          // unregister the device class
    class_destroy(ppClass);                             // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
    printk(KERN_INFO "EBBChar: Goodby from the LKM!\n");
}

// This function is called each time to the device is opened, only increments the numberOpends counter.
static int dev_open(struct inode *inodep, struct file *filep){
    numberOpens++;
    printk(KERN_INFO "PEEK-POKE: Device has been opened %d time(s)\n", numberOpens);
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    // int error_count = 0;
    // error_count = copy_to_user(buffer, message, size_of_message);
    return 0;
}

/*
    This function is called whenever the device is being written to from the user space.
    The data is copied to the message[] buffer together with the lenght
*/
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
//    char temp[len];
//    memset(temp, 0, sizeof temp);
//    copy_from_user(temp, buffer, (unsigned long) len);   
   return len;
}

// This function is called whenever the device is closed/released by the userspace progran
static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "PEEK-POKE: Device is successfully closed");
    return 0;
}

module_init(ebb_init)
module_exit(ebb_clean)