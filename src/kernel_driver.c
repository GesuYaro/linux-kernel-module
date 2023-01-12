#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/processor.h>
// #include <linux/ioctl.h>
#include <linux/ptrace.h>
// #include <linux/pid.h>
// #include <linux/netdevice.h>
// #include <asm/syscall.h>
#include <linux/sched.h>
// #include <linux/namei.h>
// #include <linux/mm_types.h>
// #include <asm/page.h>

#include "driver.h"

MODULE_LICENSE("GPL");
static int DEVICE_MAJOR_NUMBER = 26;

int pid = 0;

static struct my_thread_struct get_my_thread_struct(struct task_struct* task) {
	struct thread_struct ts = task->thread;
	pr_info("task = %d\n", task);
	pr_info("static_prio = %d", task->static_prio);
	// struct desc_struct tls_array[3] = {ts.tls_array[0], ts.tls_array[1], ts.tls_array[2]};
	struct my_thread_struct mts = {
		// .tls_array = tls_array,
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

static struct my_inode get_my_inode(struct task_struct* task) {
	struct my_inode mi = {
		.data = 206
	};
	return mi;
}

// This function will be called when we write IOCTL on the Device file
static long driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	switch(cmd) {
		case MY_REQUEST:
			pr_info("MY_REQUEST ");
			pr_info("arg = %d\n", arg);
			struct my_request mr;
			if(copy_from_user(&mr, (int*) arg, sizeof(struct my_request))) pr_err("Data write error!\n");
			pr_info("Pid = %d\n", mr.pid);
			struct task_struct* task = get_pid_task(find_get_pid(mr.pid), PIDTYPE_PID);
			struct my_thread_struct mts = get_my_thread_struct(task);
			mr.thread_struct = mts;	
			struct my_inode mi = get_my_inode(task);
			mr.inode = mi;	
			if(copy_to_user((struct my_request*) arg, &mr, sizeof(struct my_request))) {
				printk(KERN_INFO "Data read error!\n");
			}
			break;
		default:
			pr_info("Command not found!");
			// pr_info("arg = %d\n", arg);
			break;
	}
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
	printk(KERN_INFO "Core mode is finished, goodbye!\n");
}

module_init(ioctl_core_init);
module_exit(ioctl_core_exit);