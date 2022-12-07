#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ptrace.h>
#include <linux/namei.h>
#include <linux/path.h>
#include <linux/mount.h>
#include <linux/dcache.h>
#include <linux/sched/task_stack.h>
#include <asm/syscall.h>

#include "lab2-ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrew Vasiliev");
MODULE_DESCRIPTION("lab 2. ioctl: dentry, syscall_info");
MODULE_VERSION("1.0.0");


dev_t dev = 0;
static struct class *dev_class;
static struct cdev cdev_struct;


static int __init

init_driver(void);

static void __exit

exit_driver(void);

static int driver_open(struct inode *device, struct file *instance);

static int driver_close(struct inode *device, struct file *instance);

static long module_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*
** File operation sturcture
*/
static struct file_operations fops =
        {
                .owner          = THIS_MODULE,
                .open           = driver_open,
                .unlocked_ioctl = module_ioctl,
                .release        = driver_close,
        };

/*
** This function will be called when we open the Device file
*/
static int driver_open(struct inode *device, struct file *instance) {
    printk("Device file opened\n");
    return 0;
}

/*
** This function will be called when we close the Device file
*/
static int driver_close(struct inode *device, struct file *instance) {
    printk("Device file closed\n");
    return 0;
}

struct dentry *struct_dentry;

int fill_syscall_info(struct message_syscall_info *message_syscall_info) {
    struct task_struct *task = get_pid_task(find_get_pid(message_syscall_info->pid), PIDTYPE_PID);
    unsigned long args[6] = {};
    struct pt_regs *regs = task_pt_regs(task);
    message_syscall_info->sp = user_stack_pointer(regs);
    message_syscall_info->nr = syscall_get_nr(task, regs);
    message_syscall_info->instruction_pointer = instruction_pointer(regs);
    if (message_syscall_info->nr != -1L) {
        syscall_get_arguments(task, regs, args);
    }
    message_syscall_info->args[0] = args[0];
    message_syscall_info->args[1] = args[1];
    message_syscall_info->args[2] = args[2];
    message_syscall_info->args[3] = args[3];
    message_syscall_info->args[4] = args[4];
    message_syscall_info->args[5] = args[5];
    return 0;
}

int fill_dentry(struct message_dentry *message_dentry) {
    struct path path;
    char *path_name;
    path_name = message_dentry->path_name;
    if (kern_path(path_name, LOOKUP_FOLLOW, &path)) {
        printk(KERN_INFO
        "lab2-ioctl - Can't find path %s\n", path_name);
        return -1;
    }
    printk("lab2-ioctl - Update the path to %s\n", path_name);
    struct_dentry = path.dentry;
    sscanf(struct_dentry->d_parent->d_name.name, "%s", message_dentry->name);
    sscanf(struct_dentry->d_name.name, "%s", message_dentry->parent_name);
    message_dentry->i_uid = struct_dentry->d_inode->i_uid.val;
    message_dentry->i_gid = struct_dentry->d_inode->i_gid.val;
    message_dentry->d_flags = struct_dentry->d_flags;

    printk("lab2-ioctl - Dentry struct is gotten");
    return 0;
}

/*
** This function will be called when we write IOCTL on the Device file
*/
static long module_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct message_dentry message_dentry_buffer;
    struct message_syscall_info message_syscall_info_buffer;

    switch (cmd) {
        case DENTRY_RW_VALUE:
            printk(KERN_INFO
            "lab2-ioctl - start getting dentry struct\n");
            if (copy_from_user(&message_dentry_buffer, (struct message_dentry *) arg, sizeof(message_dentry_buffer))) {
                printk(KERN_INFO
                "lab2-ioctl - Error copying data from user!\n");
                break;
            }
            if (fill_dentry(&message_dentry_buffer) < 0) {
                break;
            }
            if (copy_to_user((struct message_dentry *) arg, &message_dentry_buffer, sizeof(message_dentry_buffer))) {
                printk(KERN_INFO
                "lab2-ioctl - Error copying data to user!\n");
            }
            printk("lab2-ioctl - Dentry struct is sent to user");
            printk("lab2-ioctl - end getting dentry struct\n");
            break;
        case SYSCALL_INFO_RW_VALUE:
            printk("lab2-ioctl - start getting syscall_info struct\n");

            if (copy_from_user(&message_syscall_info_buffer, (struct message_syscall_info *) arg,
                               sizeof(message_syscall_info_buffer))) {
                printk(KERN_INFO
                "lab2-ioctl - Error copying data syscall_info from user!\n");
                break;
            }
            if (!message_syscall_info_buffer.pid) {
                break;
            }
            printk(KERN_INFO
            "lab2-ioctl - pid: %d\n", message_syscall_info_buffer.pid);
            fill_syscall_info(&message_syscall_info_buffer);
            if (copy_to_user((struct message_syscall_info *) arg, &message_syscall_info_buffer,
                             sizeof(message_syscall_info_buffer))) {
                printk(KERN_INFO
                "lab2-ioctl - Error copying data to user!\n");
            }
            printk("lab2-ioctl - end getting syscall_info struct\n");
            break;
        default:
            printk("lab2-ioctl - Default\n");
            break;
    }
    return 0;
}

static int __init init_driver(void) {
    printk("lab2-ioctl - module start  work\n");

    /*Allocating Major number*/
    if ((alloc_chrdev_region(&dev, 0, 1, "lab2_dev")) < 0) {
        printk("lab2-ioctl - cannot allocate major number\n");
        return -1;
    }
    printk("lab2-ioctl -Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&cdev_struct, &fops);

    /*Adding character device to the system*/
    if ((cdev_add(&cdev_struct, dev, 1)) < 0) {
        printk("lab2-ioctl - cannot add the device to the system\n");
        goto r_class;
    }

    /*Creating struct class*/
    if ((dev_class = class_create(THIS_MODULE, "lab2_class")) == NULL) {
        printk("lab2-ioctl -cannot create the struct class lab2_class\n");
        goto r_class;
    }

    /*Creating device*/
    if ((device_create(dev_class, NULL, dev, NULL, "lab2_device")) == NULL) {
        printk("lab2-ioctl - cannot create the device lab2_device\n");
        goto r_device;
    }
    printk("lab2-ioctl - device driver inserted\n");
    return 0;

    r_device:
    class_destroy(dev_class);
    r_class:
    unregister_chrdev_region(dev, 1);
    return -1;
}

static void __exit exit_driver(void) {
    printk("lab2-ioctl - module end work\n");
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&cdev_struct);
    unregister_chrdev_region(dev, 1);
}

module_init(init_driver);
module_exit(exit_driver);



