#define MY_REQUEST _IOWR('a','a', struct my_request*)

struct my_thread_struct {
	/* Cached TLS descriptors: */
    // struct desc_struct    tls_array[3];
    unsigned long        sp;
    unsigned short        es;
    unsigned short        ds;
    unsigned short        fsindex;
    unsigned short        gsindex;
 
    unsigned long        fsbase;
    unsigned long        gsbase;
 
    /* Debug status used for traps, single steps, etc... */
    unsigned long           debugreg6;
    /* Keep track of the exact dr7 value set by the user */
    unsigned long           ptrace_dr7;
    /* Fault info: */
    unsigned long        cr2;
    unsigned long        trap_nr;
    unsigned long        error_code;
    /* IO permissions: */
    unsigned long        *io_bitmap_ptr; // from ptr to ul
    unsigned long        iopl;
    /* Max allowed port in the bitmap, in bytes: */
    unsigned        io_bitmap_max;
};

struct my_inode {
	int data;
};

struct my_request {
	int32_t pid;
	struct my_thread_struct thread_struct;
	struct my_inode inode;
};
