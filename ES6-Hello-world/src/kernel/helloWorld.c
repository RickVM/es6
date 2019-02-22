
#include <linux/kernel.h>
#include <linux/module.h>

int init_module(void)
{
    printk(KERN_INFO "ES6-APP Hello world from the kernel\n");
    return 0;
}

void cleanup_module(void) 
{
    printk(KERN_INFO "ES6-APP Goodbye world.\n");
}