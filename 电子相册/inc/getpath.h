#ifndef __GETPATH_H__
#define __GETPATH_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "LinkedListStack.h"
#include "LinkedListQueue.h"
#include "form.h"

/*
    功能:获取规定的文件路径,并存入一个队列中
    @const char *path:搜索该路径下所有符合格式的图片
    @char *FindFileStyle:文件格式
    返回值:
        队列头(Queue *)
*/
Queue * get_dir_size(const char *path,char *FindFileStyle);

#endif

