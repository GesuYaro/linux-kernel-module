# Linux kernel module
### ioctl: thread_struct, inode

**Подготовка:**
0. `cd src`
1. `sudo mknod /dev/lab2_driver c 26 1`
2. Выдать права файлу из п. 1 (например chmod)
3. `make`
4. `sudo insmod kernel_driver.ko`
5. `make user`

**Использование:**

`./user_prg.exe [ts|inode] <argument>`

ts: argument -- pid

inode: argument -- absolute path

