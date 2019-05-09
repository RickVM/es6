#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <mach/irqs.h>

#define DEVICE_NAME 		"adc"
#define ADC_NUMCHANNELS		3

// adc registers
#define	ADCLK_CTRL			LPC32XX_CLKPWR_ADC_CLK_CTRL
#define	ADCLK_CTRL1			LPC32XX_CLKPWR_ADC_CLK_CTRL_1
#define	ADC_SELECT			io_p2v(0x40048004)
#define	ADC_CTRL			io_p2v(0x40048008)
#define ADC_VALUE           io_p2v(0x40048048)
#define SIC2_ATR            io_p2v(0x40010010)
#define SIC1_ER	            io_p2v(0x4000C000)

#define READ_REG(a)         (*(volatile unsigned int *)(a))
#define WRITE_REG(b,a)      (*(volatile unsigned int *)(a) = (b))

// ADC and interrupt enable
#define AD_POWERDOWN_CTRL   1 << 2
#define TS_ADC_STROBE       1 << 1
#define ADC_VALUE_MASK      0x3FF

static unsigned char    adc_channel = 0;
static int              adc_values[ADC_NUMCHANNELS] = {0, 0, 0};
static bool             interrupt_is_gpi = false;
static bool             adc_handled = false;

DECLARE_WAIT_QUEUE_HEAD(event);

static irqreturn_t      adc_interrupt (int irq, void * dev_id);
static irqreturn_t      gp_interrupt (int irq, void * dev_id);


static void adc_init (void)
{
	unsigned long data;

	// set 32 KHz RTC clock
    data = READ_REG (ADCLK_CTRL);
    data |= 0x1;
    WRITE_REG (data, ADCLK_CTRL);

	// rtc clock ADC & Display = from PERIPH_CLK
    data = READ_REG (ADCLK_CTRL1);
    data &= ~0x01ff;
    WRITE_REG (data, ADCLK_CTRL1);

	// negatief & positieve referentie
    data = READ_REG(ADC_SELECT);
    data &= ~0x03c0;
    data |=  0x0280;
    WRITE_REG (data, ADC_SELECT);
    
    // TODO
    // aanzetten reset?? (wat is dat)
    data = READ_REG(ADC_CTRL);
	data |= AD_POWERDOWN_CTRL;
	WRITE_REG (data, ADC_CTRL);

	data = READ_REG(SIC1_ER);
	data |= IRQ_LPC32XX_TS_IRQ;
    WRITE_REG (data, SIC1_ER);

    // SET activation TYPE
    data = READ_REG(SIC2_ATR);
    data |= IRQ_LPC32XX_GPI_01;
    WRITE_REG(data, SIC2_ATR);

	//IRQ init
    if (request_irq (IRQ_LPC32XX_TS_IRQ, adc_interrupt, IRQF_DISABLED, "adc", (void*)'a') != 0)
    {
        printk(KERN_ALERT "ADC IRQ request failed\n");
    }
    if (request_irq (IRQ_LPC32XX_GPI_01, gp_interrupt, IRQF_DISABLED, "gpi", (void*)'c') != 0)
    {
        printk (KERN_ALERT "GP IRQ request failed\n");
    }
}


static void adc_start (unsigned char channel)
{
	unsigned long data;

	if (channel >= ADC_NUMCHANNELS)
    {
        channel = 0;
    }

	data = READ_REG (ADC_SELECT);

	//selecteer het kanaal, eerst uitlezen, kanaalbits negeren en dan alleen de kanaalbits veranderen (0x0030)
	WRITE_REG((data & ~0x0030) | ((channel << 4) & 0x0030), ADC_SELECT);

	//nu ook globaal zetten zodat we de interrupt kunnen herkennen
	adc_channel = channel;

	data = READ_REG(ADC_CTRL);
	data |= TS_ADC_STROBE;
    WRITE_REG (data, ADC_CTRL);
}

static irqreturn_t adc_interrupt (int irq, void * dev_id)
{
    printk(KERN_INFO "IRQ %d, dev_id: %c \n", irq, (char)dev_id);
    adc_values[adc_channel] = READ_REG(ADC_VALUE) & ADC_VALUE_MASK;
    printk(KERN_INFO "ADC(%d)=%d\n", adc_channel, adc_values[adc_channel]);

    if (interrupt_is_gpi) 
    {
        adc_channel++;
        if (adc_channel < ADC_NUMCHANNELS)
        {
            adc_start (adc_channel);
        }
        else 
        {
            interrupt_is_gpi = false;
        }
    } 
    else 
    {
        adc_handled = true;
        wake_up_interruptible(&event);
    }


    return (IRQ_HANDLED);
}

static irqreturn_t gp_interrupt(int irq, void * dev_id)
{
    printk(KERN_INFO "ADC GP INTERRUPT TRIGGERED.");
    interrupt_is_gpi = true;
    adc_start (0);
    return (IRQ_HANDLED);
}


static void adc_exit (void)
{
    free_irq (IRQ_LPC32XX_TS_IRQ, NULL);
    free_irq (IRQ_LPC32XX_GPI_01, NULL);
}


static ssize_t device_read (struct file * file, char __user * buf, size_t length, loff_t * f_pos)
{
	int channel         = (int) file->private_data;
    int toWrite         = 0;
    int written         = 0;
    char retv_buffer[128];

    printk (KERN_WARNING DEVICE_NAME ":device_read(%d)\n", channel);

    if (*f_pos > 0)
    {
        *f_pos = 0;
        return 0;
    }

    if (channel < 0 || channel >= ADC_NUMCHANNELS)
    {
		return -EFAULT;
    }

    adc_handled = false;
    adc_start (channel);
    wait_event_interruptible(event, adc_handled);

    written = sprintf(retv_buffer, "%d", adc_values[adc_channel]);
    toWrite = copy_to_user(buf, retv_buffer, written);

    if (toWrite)
    {
        written = 0;
        printk(KERN_ERR "Could not write to user space");
        return -EFAULT;
    }

    *f_pos = written;
    return (written);
}

static int device_open (struct inode * inode, struct file * file)
{
   
    int channel = MINOR(inode -> i_rdev);
    file->private_data = (void*)channel;

    try_module_get(THIS_MODULE);
    return 0;
}


static int device_release (struct inode * inode, struct file * file)
{
    printk (KERN_WARNING DEVICE_NAME ": device_release()\n");


    module_put(THIS_MODULE);
	return 0;
}


static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .read = device_read,
    .open = device_open,
    .release = device_release
};


static struct chardev
{
    dev_t       dev;
    struct cdev cdev;
} adcdev;


int adcdev_init (void)
{
    // try to get a dynamically allocated major number
	int error = alloc_chrdev_region(&adcdev.dev, 0, ADC_NUMCHANNELS, DEVICE_NAME);;

	if(error < 0)
	{
		// failed to get major number for our device.
		printk(KERN_WARNING DEVICE_NAME ": dynamic allocation of major number failed, error=%d\n", error);
		return error;
	}

	printk(KERN_INFO DEVICE_NAME ": major number=%d\n", MAJOR(adcdev.dev));

	cdev_init(&adcdev.cdev, &fops);
	adcdev.cdev.owner = THIS_MODULE;
	adcdev.cdev.ops = &fops;

	error = cdev_add(&adcdev.cdev, adcdev.dev, ADC_NUMCHANNELS);
	if(error < 0)
	{
		// failed to add our character device to the system
		printk(KERN_WARNING DEVICE_NAME ": unable to add device, error=%d\n", error);
		return error;
	}

	adc_init();

	return 0;
}


/*
 * Cleanup - unregister the appropriate file from /dev
 */
void cleanup_module()
{
	cdev_del(&adcdev.cdev);
	unregister_chrdev_region(adcdev.dev, ADC_NUMCHANNELS);

	adc_exit();
}


module_init(adcdev_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Simon Lit & Rick van Melis");
MODULE_DESCRIPTION("ADC Driver");

