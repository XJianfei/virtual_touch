/*
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>

#include <linux/input.h> 
#include <linux/miscdevice.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>

#include <linux/delay.h>

#define DEV_NAME  "vtouch"
#define INPUT_DEVICE_NAME   "virtual_touch"

#define touch_err(fmt, arg...) printk(KERN_ERR fmt, ##arg)
#define touch_dbg(fmt, arg...) printk(KERN_INFO fmt, ##arg)
#define show_line() printk(KERN_INFO "%s: [%d]\n", __func__, __LINE__)

struct virtual_touch_dev {
    struct input_dev *input;
};

static struct virtual_touch_dev ts;

static int touch_open(struct inode *inode, struct file *filp)
{
    printk("%s\n", __func__);
    return 0;
}

static int touch_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t touch_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    show_line();
    /*if (copy_to_user(buf, "123\n", 4)){*/
        /*return -EFAULT;*/
    /*}   */
    return 0;
}

ssize_t touch_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    char data[65];
    if (size > 64) {
        return -EINVAL;
    }
    if (copy_from_user(data, buf, size)) {
        return -EFAULT;
    }
    data[64] = '\0';
    input_report_abs(ts.input, ABS_X, 512);
    input_report_abs(ts.input, ABS_Y, 512);
    input_report_key(ts.input, BTN_TOUCH, 1);
    input_sync(ts.input);
    mdelay(100);
    input_report_abs(ts.input, ABS_X, 522);
    input_report_abs(ts.input, ABS_Y, 522);
    input_report_key(ts.input, BTN_TOUCH, 1);
    input_sync(ts.input);
    mdelay(100);
    input_report_abs(ts.input, ABS_X, 532);
    input_report_abs(ts.input, ABS_Y, 532);
    input_report_key(ts.input, BTN_TOUCH, 1);
    input_sync(ts.input);
    mdelay(100);
    input_report_key(ts.input, BTN_TOUCH, 0);
    input_sync(ts.input);
    touch_dbg("write: %s\n", data);
    return size;
}

static const struct file_operations touch_fops = {
    .owner = THIS_MODULE,
    .open = touch_open,
    .read = touch_read,
    .write = touch_write,
    .release = touch_release,
};

static struct miscdevice touch_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEV_NAME,
    .fops = &touch_fops,
};



static int virtual_touch_init(void)
{
    struct input_dev *input_dev;
    int ret = 0;
    printk("hi~~~!\n");
    ret = misc_register(&touch_dev);
    if (ret < 0) {
        touch_err("failed to register miscellaneous device\n");
        goto err_misc;
    }

    input_dev = input_allocate_device();
    if (!input_dev) {
        touch_err("unable to allocate the input device\n");
        ret = -ENOMEM;
        goto err_alloc;
    }
    __set_bit(EV_KEY, input_dev->evbit);
    __set_bit(EV_ABS, input_dev->evbit);
    __set_bit(EV_SYN, input_dev->evbit);

    __set_bit(BTN_TOUCH, input_dev->keybit);
    input_set_abs_params(input_dev, ABS_X, 0, 0x3FF, 0, 0);
    input_set_abs_params(input_dev, ABS_Y, 0, 0x3FF, 0, 0);
    input_dev->name = "virtual_touch";
    input_dev->id.bustype = BUS_HOST;
    input_dev->id.vendor = 0x1ff7;
    input_dev->id.product = 0x0f17;
    input_dev->id.version = 0x0001;
    
    ret = input_register_device(input_dev);
    if (ret < 0) {
        touch_err("failed to register input device\n");
        ret = -EIO;
        goto err_input;
    }

    ts.input = input_dev;
    
    return 0;
err_input:
    input_free_device(input_dev);
err_alloc:
    misc_deregister(&touch_dev);
err_misc:
    
	return ret;
}

static void virtual_touch_exit(void)
{
    printk("bye~~~!\n");
    input_unregister_device(ts.input);
    input_free_device(ts.input);
    misc_deregister(&touch_dev);
}

module_init(virtual_touch_init);
module_exit(virtual_touch_exit);

MODULE_AUTHOR("xjf");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("virtual touch input device");
//MODULE_ALIAS("platform:rtc-mstar");
