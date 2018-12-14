#include<linux/kernel.h>
#include <linux/slab.h>
#include<linux/init.h>
#include <linux/module.h>
#include<linux/sched.h>
#include <linux/fcntl.h>
#include<linux/syscalls.h>
#include <asm/uaccess.h>

#include "get_user.h"

#define  DEVICE_NAME "/dev/phone_book_kernel"
#define COMMAND_SIZE 128

asmlinkage long sys_get_user(const char* surname, unsigned int len) {

   int fd;
   char command[COMMAND_SIZE] = "get ";
   char result[COMMAND_SIZE];

   mm_segment_t old_fs = get_fs();
   set_fs(KERNEL_DS);

   printk(KERN_INFO "Hello Alexander, this is my syscall:)\n");
   
   fd = sys_open(DEVICE_NAME, O_RDWR, 10);
   if (fd > 0) {
      strncat(command, surname, len);
      sys_write(fd, command, COMMAND_SIZE);
   }

   sys_read(fd, result, COMMAND_SIZE);
   
   set_fs(old_fs);

   printk(KERN_INFO "result = %s\n", result);

   sys_close(fd);
   return 0;
}