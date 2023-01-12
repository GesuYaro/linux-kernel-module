# Linux kernel module
### ioctl: thread_struct, inode

**Подготовка:**
1. `cd src`
2. `sudo mknod /dev/lab2_driver c 26 1`
3. Выдать права файлу из п. 1 (например chmod)
4. `make`
5. `sudo insmod kernel_driver.ko`
6. `make user`

**Использование:**

`./user_prg.exe [ts|inode] <argument>`

ts: `<argument>` -- pid

inode: `<argument>` -- absolute path

