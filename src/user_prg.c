#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

#include "driver.h"


static void print_my_thread_struct(struct my_thread_struct mts) {
	printf("thread_struct:\n");
	printf("    sp: %lu\n", mts.sp);
	printf("    es: %hu\n", mts.es);
	printf("    ds: %hu\n", mts.ds);
	printf("    fsindex: %hu\n", mts.fsindex);
	printf("    gsindex: %hu\n", mts.gsindex);
	printf("    fsbase: %lu\n", mts.fsbase);
	printf("    gsbase: %lu\n", mts.gsbase);
	printf("    debugreg6: %lu\n", mts.debugreg6);
	printf("    ptrace_dr7: %lu\n", mts.ptrace_dr7);
	printf("    cr2: %lu\n", mts.cr2);
	printf("    trap_nr: %lu\n", mts.trap_nr);
	printf("    error_code: %lu\n", mts.error_code);
	printf("    io_bitmap_ptr: %" PRIXPTR "\n", (uintptr_t)mts.io_bitmap_ptr);
	printf("    iopl: %lu\n", mts.iopl);
	printf("    io_bitmap_max: %u\n", mts.io_bitmap_max);
}

static void print_my_inode(struct my_inode mi) {
	printf("inode:\n");
	printf("    i_mode: %lu\n", mi.i_mode);
	printf("    i_opflags: %hu\n", mi.i_opflags);
	printf("    i_uid: %u\n", mi.i_uid);
	printf("    i_gid: %u\n", mi.i_gid);
	printf("    i_flags: %u\n", mi.i_flags);
	printf("    i_ino: %lu\n", mi.i_ino);
	printf("    i_rdev: %lu\n", mi.i_rdev);
	printf("    i_size: %lu\n", mi.i_size);
	printf("    i_atime: %ld sec, %ld nsec\n", mi.i_atime_sec, mi.i_atime_nsec);
	printf("    i_mtime: %ld sec, %ld nsec\n", mi.i_mtime_sec, mi.i_mtime_nsec);
	printf("    i_ctime: %ld sec, %ld nsec\n", mi.i_ctime_sec, mi.i_ctime_nsec);
	printf("    i_bytes: %hu\n", mi.i_bytes);
	printf("    i_blkbits: %hu\n", mi.i_blkbits);
	printf("    i_write_hint: %hu\n", mi.i_write_hint);
	printf("    i_blocks: %lu\n", mi.i_blocks);
	printf("    dirtied_when: %lu\n", mi.dirtied_when);
	printf("    dirtied_time_when: %lu\n", mi.dirtied_time_when);
}

static int thread_struct_find(int32_t pid) {
	int fd = open("/dev/lab2_driver", O_WRONLY);
	if(fd < 0) {
		printf("Cannot open device file\n");
		return 0;
	}

    struct thread_struct_request tsr = {
        .pid = pid
    };

	ioctl(fd, RD_MY_THREAD_STRUCT, &tsr);
    printf("pid = %d\n", tsr.pid);
    print_my_thread_struct(tsr.thread_struct);

	close(fd);
	return 0;
}

static int inode_find(char* path) {
	int fd = open("/dev/lab2_driver", O_WRONLY);
	if(fd < 0) {
		printf("Cannot open device file\n");
		return 0;
	}

	struct inode_request ir = {
		.path = path
	};

	ioctl(fd, RD_MY_INODE, &ir);
	printf("path = %s\n", ir.path);
	print_my_inode(ir.inode);
	close(fd);
	return 0;
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf("The program needs an argument\n<programm-name> [ts|inode] <argument>\n");
		return -1;
	}
	if (strcmp(argv[1], "ts") == 0) {
		int32_t pid = atoi(argv[2]);
		if (pid < 1) {
			printf("Pid can be only greater than 0\n");
			return -1;
		}
		thread_struct_find(pid);
	} else if (strcmp(argv[1], "inode") == 0) {
		inode_find(argv[2]);
	} else {
		printf("No such command\n");
	}
	
}
