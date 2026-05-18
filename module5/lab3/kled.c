#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

struct timer_list my_timer;
struct tty_driver *my_driver;

static int _kbledstatus = 0;
static int test = 3;
static struct kobject *kbleds_kobject;

#define BLINK_DELAY   HZ/5
#define ALL_LEDS_ON   0x07
#define RESTORE_LEDS  0xFF

static void my_timer_func(struct timer_list *ptr)
{
    int *pstatus = &_kbledstatus;

    if (*pstatus == test)
        *pstatus = RESTORE_LEDS;
    else
        *pstatus = test;

    if (my_driver && vc_cons[fg_console].d && vc_cons[fg_console].d->port.tty) {
        (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, *pstatus);
    }

    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}

static ssize_t mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", test);
}

static ssize_t mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%d", &test);

    if (my_driver && vc_cons[fg_console].d && vc_cons[fg_console].d->port.tty) {
         (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, test);
         _kbledstatus = test;
    }

    return count;
}

static struct kobj_attribute mode_attribute = __ATTR(mode, 0660, mode_show, mode_store);

static int __init kbleds_init(void)
{
    int i;
    int error;

    printk(KERN_INFO "kbleds: loading\n");
    printk(KERN_INFO "kbleds: fgconsole is %x\n", fg_console);

    kbleds_kobject = kobject_create_and_add("kbleds_control", kernel_kobj);
    if (!kbleds_kobject) {
        printk(KERN_ERR "kbleds: failed to create kobject\n");
        return -ENOMEM;
    }

    error = sysfs_create_file(kbleds_kobject, &mode_attribute.attr);
    if (error) {
        printk(KERN_ERR "kbleds: failed to create the mode file in /sys/kernel/kbleds_control\n");
        kobject_put(kbleds_kobject);
        return error;
    }

    for (i = 0; i < MAX_NR_CONSOLES; i++) {
        if (!vc_cons[i].d)
            break;
        printk(KERN_INFO "kbleds: console[%i/%i] #%i, tty %lx\n", i,
               MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
               (unsigned long)vc_cons[i].d->port.tty);
    }
    printk(KERN_INFO "kbleds: finished scanning consoles\n");

    if (!vc_cons[fg_console].d || !vc_cons[fg_console].d->port.tty) {
        printk(KERN_ERR "kbleds: Cannot get tty driver\n");
        kobject_put(kbleds_kobject);
        return -ENODEV;
    }

    my_driver = vc_cons[fg_console].d->port.tty->driver;

    timer_setup(&my_timer, my_timer_func, 0);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);

    return 0;
}

static void __exit kbleds_cleanup(void)
{
    printk(KERN_INFO "kbleds: unloading...\n");

    del_timer_sync(&my_timer);

    if (my_driver && vc_cons[fg_console].d && vc_cons[fg_console].d->port.tty) {
        (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
    }

    sysfs_remove_file(kbleds_kobject, &mode_attribute.attr);
    kobject_put(kbleds_kobject);
}

MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs with SysFS control.");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alina");

module_init(kbleds_init);
module_exit(kbleds_cleanup);