/*
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/platform_device.h>
#include <linux/string.h>

#include <linux/input.h> 
#include <linux/input/mt.h>
#include <linux/miscdevice.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>

#include <linux/delay.h>
#include <asm/uaccess.h>

#define DEV_NAME  "vtouch"
#define INPUT_DEVICE_NAME   "virtual_touch"

#define touch_err(fmt, arg...) printk(KERN_ERR fmt, ##arg)
#define touch_dbg(fmt, arg...) printk(KERN_INFO fmt, ##arg)
#define show_line() printk(KERN_INFO "%s: [%d]\n", __func__, __LINE__)

struct virtual_touch_dev {
    struct input_dev *input;
};

static struct virtual_touch_dev ts;
static int maxcontacts = 1;

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

static int strtoul(const char *s, unsigned int base, unsigned long *res)
{
    int i;
    char *src = s;
    char num[32];
    i = 0;
    while (*src != '\0' && *src != ' ' && 
            *src >= '0' && *src <= '9') {
        num[i] = *src;
        i++;
        src++;
    }
    num[i] = '\0';
    return kstrtoul(num, base, res);
}

static int strtoula(const char *s, unsigned int base, unsigned long *res, int size)
{
    char *cur = s;
    int ret, value;
    int count = 0;
    while (size) {
        ret = strtoul(cur, base, &value);
        if (ret < 0) {
            return count;
        }
        res[count] = value;
        /*printk("[%d]", res[count]);*/
        count++;
        size--;
        cur = strstr(cur, " ");
        if (cur == NULL) {
            return count;
        }
        cur++;
    }
    return count;
}

ssize_t touch_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    char data[65];
    char userdata[256];
    char *token, *cur;
    int i, ret;
    int coor[10][2];
    unsigned long value;
#if 1
    if (size > 256) {
        return -EINVAL;
    }
    if (copy_from_user(userdata, buf, size)) {
        return -EFAULT;
    }
    cur = userdata;
    printk("---------------------------------\n");
    unsigned long co[40];
    ret = strtoula(userdata, 10, co, 40);
#if 1
    printk("strtoula reteurn %d\n", ret);
    for (i = 0; i < ret; i++) {
        printk("|%d|", co[i]);
    }
#endif

#if 1
        /*printk("----i = %d\n", i);*/
    i = 0;
    int point = ret >> 2;
    while (i < point) {
        /*printk("i = %d\n", i);*/
        input_report_key(ts.input, BTN_TOUCH, co[(i << 2) + 1]);
        input_report_abs(ts.input, ABS_MT_TRACKING_ID, co[i << 2]);
        input_event(ts.input, EV_ABS, ABS_MT_POSITION_X, co[(i << 2) + 2]);
        input_event(ts.input, EV_ABS, ABS_MT_POSITION_Y, co[(i << 2) + 3]);
        i++;
        input_mt_sync(ts.input);
    }
    input_sync(ts.input);
    /*i = 0;*/
    /*while (i < ret) {*/
        /*[>printk("i = %d\n", i);<]*/
        /*input_report_key(ts.input, BTN_TOUCH, 0);*/
        /*input_report_abs(ts.input, ABS_MT_TRACKING_ID, i + 12);*/
        /*input_event(ts.input, EV_ABS, ABS_MT_POSITION_X, coor[i][0]);*/
        /*input_event(ts.input, EV_ABS, ABS_MT_POSITION_Y, coor[i][1]);*/
        /*i++;*/
        /*input_mt_sync(ts.input);*/
    /*}*/
    /*input_sync(ts.input);*/
#endif
#if 0
    for (i = 0; i < maxcontacts; i++) {
        input_report_key(ts.input, BTN_TOUCH, 1);
        input_report_abs(ts.input, ABS_MT_TRACKING_ID, i + 12);
        input_event(ts.input, EV_ABS, ABS_MT_POSITION_X, 512 + i * 100);
        input_event(ts.input, EV_ABS, ABS_MT_POSITION_Y, 512 + i * 100);
        input_mt_sync(ts.input);
    }
    input_sync(ts.input);

    for (i = 0; i < maxcontacts; i++) {
        input_report_key(ts.input, BTN_TOUCH, 0);
        input_report_abs(ts.input, ABS_MT_TRACKING_ID, i + 12);
        input_event(ts.input, EV_ABS, ABS_MT_POSITION_X, 512 + i * 100);
        input_event(ts.input, EV_ABS, ABS_MT_POSITION_Y, 512 + i * 100);
        input_mt_sync(ts.input);
    }
    input_sync(ts.input);
    maxcontacts--;
    if (maxcontacts == 0) {
        maxcontacts = 10;
    }
#endif

#if 0
    for (i = 0; i < maxcontacts; i++) {
        input_mt_slot(ts.input, i);
        /*touch_dbg("... %i\n", i);*/
        input_mt_report_slot_state(ts.input, MT_TOOL_FINGER, 1);
        input_event(ts.input, EV_ABS, ABS_MT_POSITION_X, 512 + i * 100);
        input_event(ts.input, EV_ABS, ABS_MT_POSITION_Y, 512 + i * 100);
        input_event(ts.input, EV_ABS, ABS_MT_PRESSURE, 100);
    }
    input_mt_report_pointer_emulation(ts.input, true);
    input_sync(ts.input);

    for (i = 0; i < maxcontacts; i++) {
        input_mt_slot(ts.input, i);
        /*touch_dbg("... %i\n", i);*/
        input_mt_report_slot_state(ts.input, MT_TOOL_FINGER, 0);
        /*input_event(ts.input, EV_ABS, ABS_MT_POSITION_X, 512 + i * 100);*/
        /*input_event(ts.input, EV_ABS, ABS_MT_POSITION_Y, 512 + i * 100);*/
    }
    input_mt_report_pointer_emulation(ts.input, true);
    input_sync(ts.input);
#endif

#endif //
#if 0
    /* single touch  */
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
#endif
    /*touch_dbg(".. write: %s\n\n", data);*/
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
    input_set_abs_params(input_dev, ABS_X, 0, 1920, 0, 0);
    input_set_abs_params(input_dev, ABS_Y, 0, 1080, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_POSITION_X, 0, 1920, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_POSITION_Y, 0, 1080, 0, 0);
    /*input_set_abs_params(input_dev, ABS_MT_PRESSURE, 0, 0x3FF, 0, 0);*/
    maxcontacts = 10;
    /*input_mt_init_slots(input_dev, maxcontacts);*/
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
