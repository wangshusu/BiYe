#ifndef __FORM_H__
#define __FORM_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>


/*
    判断文件类型是够与 参数Mystype相同
    相同返回 true
    不相同返回 false
    @const char * Mytype 自定义文件类型
*/
bool form(const char * buf,const char * Mytype);


#endif 