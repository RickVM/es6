#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/completion.h>
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
#define SIC1_ER	            LPC32XX_SIC1_BASE

#define READ_REG(a)         (*(volatile unsigned int *)(a))
#define WRITE_REG(b,a)      (*(volatile unsigned int *)(a) = (b))

// ADC and interrupt enable
#define AD_POWERDOWN_CTRL   1 << 2
#define TS_ADC_STROBE       1 << 1
#define ADC_VALUE_MASK      0x000003FF
#define GPI1_EDGE           1 << 23

static irqreturn_t      adc_interrupt (int irq, void * dev_id);
static irqreturn_t      gp_interrupt (int irq, void * dev_id);

struct state {
    int                 adc_values[ADC_NUMCHANNELS];
    bool                interrupt_is_gpi;
    unsigned char       adc_channel;
    struct completion   completion;
    struct mutex        mlock;
};

static struct state st;

static void adc_init (void)
{
	unsigned long data;
    st.adc_values[0] = 0;
    st.adc_values[1] = 0;
    st.adc_values[2] = 0;
    st.adc_channel = 0;
    
    init_completion(&st.completion);
    mutex_init(&st.mlock);

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
    
    //Turn on ADC
    data = READ_REG(ADC_CTRL);
	data |= AD_POWERDOWN_CTRL;
	WRITE_REG (data, ADC_CTRL);

    // Enable Touchscreen interrupt
	data = READ_REG(SIC1_ER);
	data |= IRQ_LPC32XX_TS_IRQ;
    WRITE_REG (data, SIC1_ER);

    // SET activation TYPE
    data = READ_REG(SIC2_ATR);
    data |= GPI1_EDGE;
    WRITE_REG(data, SIC2_ATR);

    if (request_irq (IRQ_LPC32XX_TS_IRQ, adc_interrupt, IRQF_DISABLED, "adc", NULL) != 0)
    {
        printk(KERN_ALERT "ADC IRQ request failed\n");
    }
    if (request_irq (IRQ_LPC32XX_GPI_01, gp_interrupt, IRQF_DISABLED, "gpi", NULL) != 0)
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

	st.adc_channel = channel;

	data = READ_REG(ADC_CTRL);
	data |= TS_ADC_STROBE;
    WRITE_REG (data, ADC_CTRL);
}

static irqreturn_t adc_interrupt (int irq, void * dev_id)
{
    st.adc_values[st.adc_channel] = READ_REG(ADC_VALUE) & ADC_VALUE_MASK;
    printk(KERN_INFO "ADC(%d)=%d\n", st.adc_channel, st.adc_values[st.adc_channel]);

    if (st.interrupt_is_gpi) 
    {
        st.adc_channel++;
        if (st.adc_channel < ADC_NUMCHANNELS)
        {
            adc_start (st.adc_channel);
        }
        else 
        {
            st.interrupt_is_gpi = false;

            // RESET TEST PIN

            unsigned long* memAddr = 0;
            memAddr = io_p2v(0x40028024);
            *memAddr |= (1UL << 5);
        }
    } 
    else 
    {
        complete(&st.completion);
    }
    return (IRQ_HANDLED);
}

static irqreturn_t gp_interrupt(int irq, void * dev_id)
{
    // TESTING SPEED OF GP_INTERRUPT
    // Setting pin{ "J3", 58, 5, 5, &pinctrl_port2 } P2.5
    unsigned long* memAddr = 0;
    memAddr = io_p2v(0x40028020);
    *memAddr |= (1UL << 5);


    st.interrupt_is_gpi = true;
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

    mutex_lock(&st.mlock);

    adc_start (channel);
    wait_for_completion(&st.completion);
    written = sprintf(retv_buffer, "%d", st.adc_values[st.adc_channel]);

    mutex_unlock(&st.mlock);

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

