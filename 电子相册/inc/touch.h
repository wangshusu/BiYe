#ifndef __TOUCH_H__
#define __TOUCH_H__

#include <linux/input.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "lcd.h"
#include "LinkedListQueue.h"
#include "showphoto.h"


#define Touch_Path "/dev/input/event0"

enum Move_Dir//描述屏幕的操作的枚举
{
    RIGHT,//右滑
    LIFT,//左滑
    UP,//上滑
    DOWN,//下滑
    DBCLICK,//双击
    CLICK,//单击
    DIR_ERROR//错误
};

/*
    @enum Move_Dir GetMove_Dir():获取屏幕操作函数
    @无需输入
    返回值:
        返回enum Move_Dir里面的方向判断
*/
enum Move_Dir GetMove_Dir(Node *ptr);

/*
    @enum Move_Dir Decide_Dir:判断方向函数
    @输入 起始 和 终止 的(x,y)坐标
    返回值:
        返回enum Move_Dir里面的方向判断
*/
enum Move_Dir Decide_Dir(int x0,int x1,int y0,int y1);

#endif