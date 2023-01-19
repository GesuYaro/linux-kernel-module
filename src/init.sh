#!/bin/bash
sudo mknod /dev/lab2_driver c 26 1
sudo chmod 777 /dev/lab2_driver
sudo insmod kernel_driver.ko 

