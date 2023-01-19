obj-m += kernel_driver.o
 
PWD := $(CURDIR)
 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

user:
	gcc -pedantic-errors -Wall -Werror -g3 -O0 --std=c99 ./user_prg.c -o user_prg.exe
