#ifndef __TOUCH_H__
#define __TOUCH_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include<pthread.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/input.h>
#include <sys/ioctl.h>


#define EVENT0      "/dev/input/event0"
int init_Ts();
bool ts_display(int fd_ts,int *x,int *y);
void Ts_exit(int fd);


#endif
