#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define SIZE_BUFF 128
#define NAME_NODE "mymod"

int len, temp;

char *msg = NULL;

static ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *ppos)
{
    if (count > temp)
    {
        count = temp;
    }

    temp = temp - count;

    printk(KERN_INFO "READ: %ld\n", count);

    if (count == 0)
    {
        temp = len;
        return 0;
    }

    int res = copy_to_user(buf, msg, count);

    if (res < 0)
    {
        printk(KERN_INFO "MESSAGE NOT READ!\n");
    }

    printk(KERN_INFO "RETURN BYTES: %d\n", res);

    if (count == 0)
    {
        temp = len;
    }

    return count;
}

static ssize_t write_proc(struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
    printk(KERN_INFO "WRITE: %ld\n", count);
    int res = copy_from_user(msg, buf, count);
    if (res < 0)
    {
        printk(KERN_INFO "MESSAGE NOT WRITTEN!\n");
    }

    len  = count;
    temp = len;
    printk(KERN_INFO "RETURN BYTES: %d\n", res);
    return count;
}

static const struct proc_ops proc_ops = {.proc_read = read_proc, .proc_write = write_proc};

static void create_new_proc_entry(void)
{
    struct proc_dir_entry *entry;

    entry = proc_create(NAME_NODE, 0, NULL, &proc_ops);
    if (!entry)
        return;

    msg = kmalloc(SIZE_BUFF * sizeof(char), GFP_KERNEL);
}

static int __init proc_module_init(void)
{
    create_new_proc_entry();

    printk(KERN_INFO "CREATE MODULE: Succsess!\n");
    return 0;
}

static void __exit proc_module_exit(void)
{
    remove_proc_entry(NAME_NODE, NULL);
    kfree(msg);
    printk(KERN_INFO "/proc/%s REMOVED: Success!\n", NAME_NODE);
}

module_init(proc_module_init);
module_exit(proc_module_exit);

MODULE_LICENSE("ALELE");
MODULE_AUTHOR("Alina");
