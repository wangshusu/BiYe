#ifndef __BMP_H__
#define __BMP_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


/*
    显示bmp图片函数
    @char *bmp_file:路径
    @int x0, int y0 : 在哪个点显示图片
    @int cmd:显示特效
    无返回值
*/
void display_bmp(char *bmp_file, int x0, int y0, int cmd);


#endif