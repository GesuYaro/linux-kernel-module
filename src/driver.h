#define RD_MY_THREAD_STRUCT _IOWR('a','a', struct thread_struct_request*)
#define RD_MY_INODE _IOWR('a','b', struct inode_request*)

struct my_thread_struct {
	/* Cached TLS descriptors: */
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
	unsigned long	i_mode;
	unsigned short	i_opflags;
	unsigned int	i_uid;
	unsigned int	i_gid;
	unsigned int	i_flags;

    unsigned long	i_ino;

    unsigned long	i_rdev;
	unsigned long	i_size;
	long	i_atime_sec;
	long	i_atime_nsec;
	long	i_mtime_sec;
	long	i_mtime_nsec;
	long	i_ctime_sec;
	long	i_ctime_nsec;
	unsigned short      i_bytes;
	unsigned short	    i_blkbits;
	unsigned short		i_write_hint;
	uint64_t		i_blocks;
    unsigned long		dirtied_when;	/* jiffies of first dirtying */
	unsigned long		dirtied_time_when;
};

struct thread_struct_request {
	int32_t pid;
	struct my_thread_struct thread_struct;
};

struct inode_request {
    char* path;
    struct my_inode inode;
};
