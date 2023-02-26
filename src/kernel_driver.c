#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/processor.h>
#include <linux/namei.h>
#include <linux/fs.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/ioctl.h>
#include <linux/err.h>
#include <linux/mutex.h>

#include "driver.h"

MODULE_LICENSE("GPL");
DEFINE_MUTEX(zhopa);
DEFINE_MUTEX(zhopa2);

dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_driver_cdev;

static struct my_thread_struct get_my_thread_struct(struct task_struct* task) {
	struct thread_struct ts = task->thread;
	pr_info("task = %d\n", task);
	struct my_thread_struct mts = {
		.sp = ts.sp,
		.es = ts.es,
		.ds = ts.ds,
		.fsindex = ts.fsindex,
		.gsindex = ts.gsindex,
		.fsbase = ts.fsbase,
		.gsbase = ts.gsbase,
		.debugreg6 = ts.debugreg6,
		.ptrace_dr7 = ts.ptrace_dr7,
		.cr2 = ts.cr2,
		.trap_nr = ts.trap_nr,
		.error_code = ts.error_code,
		.io_bitmap_ptr = ts.io_bitmap_ptr,
		.iopl = ts.iopl,
		.io_bitmap_max = ts.io_bitmap_max
	};
	return mts;
}

static struct my_inode get_my_inode(struct inode *inode) {
	struct my_inode mi = {
			.i_mode = inode->i_mode,
			.i_opflags = inode->i_opflags,
			.i_uid = inode->i_uid.val,
			.i_gid = inode->i_gid.val,
			.i_flags = inode->i_flags,
			.i_ino = inode->i_ino,
			.i_rdev = inode->i_rdev,
			.i_size = inode->i_size,
			.i_atime_sec = inode->i_atime.tv_sec,
			.i_atime_nsec = inode->i_atime.tv_nsec,
			.i_mtime_sec = inode->i_mtime.tv_sec,
			.i_mtime_nsec = inode->i_mtime.tv_nsec,
			.i_ctime_sec = inode->i_ctime.tv_sec,
			.i_ctime_nsec = inode->i_ctime.tv_nsec,
			.i_bytes = inode->i_bytes,
			.i_blkbits = inode->i_blkbits,
			.i_write_hint = inode->i_write_hint,
			.i_blocks = inode->i_blocks,
			.dirtied_when = inode->dirtied_when,
			.dirtied_time_when = inode->dirtied_time_when
	};
	return mi;
}

static int my_driver_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...\n");
        return 0;
}

static int my_driver_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...\n");
        return 0;
}

static ssize_t my_driver_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read Function\n");
        return 0;
}

static ssize_t my_driver_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write function\n");
        return len;
}

// This function will be called when we write IOCTL on the Device file
static long driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	long flags;
	switch(cmd) {
		case RD_MY_THREAD_STRUCT:
			pr_info("RD_MY_THREAD_STRUCT ");
			pr_info("arg = %d\n", arg);
			struct thread_struct_request tsr;
			mutex_lock(&zhopa);
			if(copy_from_user(&tsr, (int*) arg, sizeof(struct thread_struct_request))) pr_err("Data write error!\n");
			pr_info("Pid = %d\n", tsr.pid);
			mutex_unlock(&zhopa);
			struct task_struct* task = get_pid_task(find_get_pid(tsr.pid), PIDTYPE_PID);
			if (!task) {
				printk(KERN_INFO "PID not found!\n");
				break;
			}
			struct my_thread_struct mts = get_my_thread_struct(task);
			tsr.thread_struct = mts;
			mutex_lock(&zhopa);	
			if(copy_to_user((struct thread_struct_request*) arg, &tsr, sizeof(struct thread_struct_request))) {
				printk(KERN_INFO "Data read error!\n");
			}
			mutex_unlock(&zhopa);
			break;
		case RD_MY_INODE:
			pr_info("RD_MY_INODE ");
			pr_info("arg = %d\n", arg);
			struct inode_request* ir = vmalloc(sizeof(struct inode_request));
			mutex_lock(&zhopa2);
			if(copy_from_user(ir, (struct inode_request*) arg, sizeof(struct inode_request))) pr_err("Data write error!\n");
			mutex_unlock(&zhopa2);
			pr_info("Path = %s", ir->path);
			struct path path_struct;
			int err = kern_path(ir->path, LOOKUP_EMPTY, &path_struct);
			if (!err) {
				struct inode* inode = path_struct.dentry->d_inode;
				struct my_inode mi = get_my_inode(inode);
				ir->inode = mi;
				if(copy_to_user((struct inode_request*) arg, ir, sizeof(struct inode_request))) {
					printk(KERN_INFO "Data read error!\n");
				}
			} else {
				ir->inode = (struct my_inode){0};
				pr_info("Wrong path!");
			}	
			mutex_lock(&zhopa2);
			if(copy_to_user((struct inode_request*) arg, ir, sizeof(struct inode_request))) {
					printk(KERN_INFO "Data read error!\n");
			}
			mutex_unlock(&zhopa2);
			break;
		default:
			pr_info("Command not found!");
			break;
	}
	return 0;
}

static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = my_driver_read,
        .write          = my_driver_write,
        .open           = my_driver_open,
        .unlocked_ioctl = driver_ioctl,
        .release        = my_driver_release,
};

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "my_driver")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&my_driver_cdev, &fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&my_driver_cdev, dev, 1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create(THIS_MODULE, "my_driver_class"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(dev_class, NULL, dev, NULL, "lab2_driver"))){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("Device Driver Insert...Done\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev, 1);
        return -1;
}

/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&my_driver_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
