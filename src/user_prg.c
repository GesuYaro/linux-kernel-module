#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

#include "driver.h"

// static void print_desc_struct(struct desc_struct ds) {
// 	printf("        desc_srtuct:\n            limit0 = %u\n            base0 = %u)", ds.limit0, ds.base0);
// }

static void print_my_thread_struct(struct my_thread_struct mts) {
	printf("thread_struct:\n");
	// printf("    tls_array[0]:\n")
	// print_desc_struct(mts.tls_array[0]);
	// printf("    tls_array[1]:\n")
	// print_desc_struct(mts.tls_array[1]);
	// printf("    tls_array[2]:\n")
	// print_desc_struct(mts.tls_array[2]);
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

int main(int argc, char *argv[]) {
	int fd;
	if(argc < 2) {
		printf("The program needs an argument - a pid!\n");
		return -1;
	}
	int32_t pid = atoi(argv[1]);
	if (pid < 1) {
		printf("Pid can be only greater than 0\n");
		return -1;
	}
	printf("\nOpening a driver...\n");
	fd = open("/dev/lab2_driver", O_WRONLY);
	if(fd < 0) {
		printf("Cannot open device file\n");
		return 0;
	}

    struct my_request mr = {
        .pid = pid
    };

	ioctl(fd, MY_REQUEST, &mr);
    printf("pid = %d\n", mr.pid);
    print_my_thread_struct(mr.thread_struct);
    printf("inode = %d\n", mr.inode.data);

	close(fd);
}