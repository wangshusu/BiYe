#include "form.h"

bool form(const char * buf,const char * Mytype)
{
    const char * pfile;
    if(buf == NULL)//没有参数
    {
        printf("no buf!!\n");
        return false;
    }

    pfile = strrchr(buf, '.');

    if(pfile != NULL)
    {
        if(strcmp(pfile,Mytype) == 0)
        {
            //printf("%s\n",Mytype);
            return true;
        }
    }
    //printf("no find\n");
    return false;
}