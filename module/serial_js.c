#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/fs.h>           /* Needed for register_chrdev */
#include <linux/input.h>        /* Needed for create input device */
#include <linux/cdev.h>         /* Needed for cdev functions */
#include <linux/kdev_t.h>

#define NUM_OF_CONTROLLERS 4
#define DEV_NAME "serial_js" 
#define FIX 42

static int major;
static struct class *cl;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static struct input_dev * new_input_device(void)
{
	int i;
	int err;
	struct input_dev *input_dev;
	
	input_dev = input_allocate_device();
	if (!input_dev)
	{
		pr_err("Not enough memory for input device\n");
		return NULL;
	}

	input_dev->name = "Serial joystick emulator";
	input_dev->phys = "input0";
	input_dev->id.bustype = BUS_PARPORT;
	input_dev->id.vendor = 0x0001;
	input_dev->id.product = 0;
	input_dev->id.version = 0x0100;
	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);

	__set_bit(BTN_A, input_dev->keybit);
	__set_bit(BTN_B, input_dev->keybit);
	__set_bit(BTN_X, input_dev->keybit);
	__set_bit(BTN_Y, input_dev->keybit);
	__set_bit(BTN_Z, input_dev->keybit);
	__set_bit(BTN_TL, input_dev->keybit);
	__set_bit(BTN_TR, input_dev->keybit);
	__set_bit(BTN_START, input_dev->keybit);

	for (i = 0; i < 2; i++)
	{
	    input_set_abs_params(input_dev, ABS_X + i, FIX, 255 - FIX, 0, 15);
	    input_set_abs_params(input_dev, ABS_RX + i, FIX, 255 - FIX, 0, 15);
	    input_set_abs_params(input_dev, ABS_GAS + i,
			    -255 + FIX, 255 - FIX, 0, 40);
	    input_set_abs_params(input_dev, ABS_HAT0X + i, -1, 1, 0, 0);
	}

	err = input_register_device(input_dev);
	if (err)
	{
		input_free_device(input_dev);
		printk(KERN_INFO "Error registering input device\n");
	}

	return input_dev;
}

static int device_open(struct inode *inode, struct file *file)
{
	file->private_data = new_input_device();

	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	struct input_dev *input_dev = (struct input_dev*)file->private_data;

	input_unregister_device(input_dev);

	return 0;
}

static ssize_t device_read(struct file *file, char *out, size_t size, loff_t *offset)
{
	return 0;
}

static const int buttonMap[] =
    {BTN_START, BTN_Y, BTN_X, BTN_B, BTN_A,
     BTN_TL, BTN_TR, BTN_Z};

static ssize_t device_write(struct file *file, const char *out, size_t size, loff_t *offset)
{
	int i;
	long axis;
	char outCopy[36];

	struct input_dev *input_dev = (struct input_dev*)file->private_data;

	if (size != 35)
	{
		printk(KERN_ERR "Wrong size %ld\n", size);
		return -EINVAL;
	}
	else
	{
		memcpy(outCopy, out, 35);
		outCopy[15] = outCopy[19] = outCopy[23] =
			outCopy[27] = outCopy[31] = outCopy[35] = '\0';
	}

	for (i = 0; i < 8; i++)
	{
		input_report_key(input_dev, buttonMap[i], (out[i] & 0x20));
	}

	input_report_abs(input_dev, ABS_HAT0Y, (out[8] & 0x20) ? 1 :
			((out[9] & 0x20) ? -1 : 0));
	input_report_abs(input_dev, ABS_HAT0X, (out[10] & 0x20) ? 1 :
			((out[11] & 0x20) ? -1 : 0));

	kstrtol(outCopy + 12, 10, &axis);
	input_report_abs(input_dev, ABS_X, axis);
	kstrtol(outCopy + 16, 10, &axis);
	input_report_abs(input_dev, ABS_Y, axis);
	kstrtol(outCopy + 20, 10, &axis);
	input_report_abs(input_dev, ABS_RX, axis);
	kstrtol(outCopy + 24, 10, &axis);
	input_report_abs(input_dev, ABS_RY, axis);
	kstrtol(outCopy + 28, 10, &axis);
	input_report_abs(input_dev, ABS_GAS, axis);
	kstrtol(outCopy + 32, 10, &axis);
	input_report_abs(input_dev, ABS_BRAKE, axis);
	
	input_sync(input_dev);

	return size;
}

static int __init serial_js_init(void)
{
	int i;

	major = register_chrdev(0, DEV_NAME, &fops);
	if (major < 0) 
	{
		printk(KERN_WARNING "Error allocating chrdev\n");
		return -1;
	}

	cl = class_create(THIS_MODULE, DEV_NAME);

	for (i = 0; i < NUM_OF_CONTROLLERS; i++)
	{
		device_create(cl, NULL, MKDEV(major, i), NULL, DEV_NAME "%d", i);
	}

        return 0;
}

static void __exit serial_js_exit(void)
{
	int i;

	for (i = 0; i < NUM_OF_CONTROLLERS; i++)
	{
		device_destroy(cl, MKDEV(major, i));
	}
	class_unregister(cl);
	class_destroy(cl);
	unregister_chrdev(major, DEV_NAME);
        //printk(KERN_INFO "Terminating serial_js\n");
}

module_init(serial_js_init);
module_exit(serial_js_exit);

MODULE_LICENSE("GPL");
