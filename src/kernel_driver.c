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

#include "driver.h"

MODULE_LICENSE("GPL");
DEFINE_SPINLOCK(my_spinlock);
static int DEVICE_MAJOR_NUMBER = 26;

int pid = 0;

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

// This function will be called when we write IOCTL on the Device file
static long driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	long flags;
	spin_lock_irqsave(&my_spinlock, flags);
	switch(cmd) {
		case RD_MY_THREAD_STRUCT:
			pr_info("RD_MY_THREAD_STRUCT ");
			pr_info("arg = %d\n", arg);
			struct thread_struct_request tsr;
			if(copy_from_user(&tsr, (int*) arg, sizeof(struct thread_struct_request))) pr_err("Data write error!\n");
			pr_info("Pid = %d\n", tsr.pid);
			struct task_struct* task = get_pid_task(find_get_pid(tsr.pid), PIDTYPE_PID);
			struct my_thread_struct mts = get_my_thread_struct(task);
			tsr.thread_struct = mts;	
			if(copy_to_user((struct thread_struct_request*) arg, &tsr, sizeof(struct thread_struct_request))) {
				printk(KERN_INFO "Data read error!\n");
			}
			break;
		case RD_MY_INODE:
			pr_info("RD_MY_INODE ");
			pr_info("arg = %d\n", arg);
			struct inode_request ir;
			if(copy_from_user(&ir, (int*) arg, sizeof(struct inode_request))) pr_err("Data write error!\n");
			pr_info("Path = %s", ir.path);
			struct inode *inode;
			struct path path_struct;
			kern_path(ir.path, LOOKUP_FOLLOW, &path_struct);
			inode = path_struct.dentry->d_inode;
			struct my_inode mi = get_my_inode(inode);
			ir.inode = mi;
			if(copy_to_user((struct inode_request*) arg, &ir, sizeof(struct inode_request))) {
				printk(KERN_INFO "Data read error!\n");
			}
			break;
		default:
			pr_info("Command not found!");
			break;
	}
	spin_unlock_irqrestore(&my_spinlock, flags);
	return 0;
}

static struct file_operations fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = driver_ioctl,
};

static int __init ioctl_core_init(void) {
	printk(KERN_INFO "Driver started\n");
	int retval;
	retval = register_chrdev(DEVICE_MAJOR_NUMBER, "my_driver", &fops);
	if (0 == retval) {
		printk("my_driver device number Major:%d , Minor:%d\n", DEVICE_MAJOR_NUMBER, 0);
	} else if (retval > 0) {
		printk("my_driver device number Major:%d , Minor:%d\n", retval >> 20, retval & 0xffff);
	} else {
		printk("Couldn't register device number!\n");
		return -1;
	}
	return 0;
}

static void __exit ioctl_core_exit(void) {
	unregister_chrdev(DEVICE_MAJOR_NUMBER, "my_driver");
	printk(KERN_INFO "Closing the driver!\n");
}

module_init(ioctl_core_init);
module_exit(ioctl_core_exit);