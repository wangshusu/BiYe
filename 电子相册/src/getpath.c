/*
 * @FilePath: \3.电子相册\src\getpath.c
 * @Brief: 
 */
#include "getpath.h"

Queue * get_dir_size(const char *path,char *FindFileStyle)
{
    //先打开输入的 argv[1] 的这个目录
    DIR *dir1 = opendir(path);
    if(dir1 == NULL)
    {
        perror("打开目录失败");
        return NULL;
    }

    char pathname[512] = {0};//输入路径

    struct stat st;//用来保存文件属性
    int dir_size = 0;//文件的总长度
    struct dirent *p = NULL;//用来保存 readdir 读取后返回的 dir

    Stack * stack = InitStack();//初始化栈
    Queue *qu = InitQueue();//初始化队列

    if(stack == NULL || qu == NULL)//如果创建失败返回 -1
    {
        printf("st or qu can't creat\n");
        return NULL;
    }

    //开始读取文件
    while((p = readdir(dir1)))
    {
        char *pwd = NULL;//保存相对路径
        memset(pathname,0,sizeof(pathname));
        sprintf(pathname,"%s/%s",path,p->d_name);//获取文件名
        pwd = calloc(1,strlen(pathname) + 1);
        if(pwd == NULL)
        {
            printf("pwd calloc faile\n");
        }
        memcpy(pwd,pathname,strlen(pathname));//将文件名传给 pwd

        stat(pwd,&st);//将 path 的文件属性赋值 给st

        
        if(strcmp(p->d_name,".") == 0 || strcmp(p->d_name,"..") == 0)
        {

        }
        else//如果是文件
        {
            //printf("                pwd = %s\n",pwd);
            if(S_ISREG(st.st_mode))//如果是普通文件
            {
                if(form(pathname,FindFileStyle))
                {
                    InQueue(qu,(MyStackType)pwd);	
                    dir_size += 1;
                }
            }
            else//如果是 文件夹
            {
                InStack(stack,(MyStackType)pwd);
            } 

        }
        
    }

    closedir(dir1);//关闭目录

    
    while(stack ->num != 0)//只要里面还有元素就继续出栈
    {
        char * pwd1 = (char *)OutStack(stack);
        //printf("%s\n",pwd1);
        DIR *dir1 = opendir(pwd1);
        if(dir1 == NULL)
        {
            perror("打开目录失败");
            return NULL;
        }
        while(p = readdir(dir1))
        {
            if(strcmp(p->d_name,".") == 0 || strcmp(p->d_name,"..") == 0)
            {
                //如果 是 . 或者 .. 那就不管 接着往下走
            }
            else//如果是文件
            {
                char *pwd2;
                memset(pathname,0,sizeof(pathname));//清空
                sprintf(pathname,"%s/%s",pwd1,p->d_name);//获取文件名
                
                pwd2 = calloc(1,strlen(pathname) + 1);
                if(pwd2 == NULL)
                {
                    printf("pwd calloc faile\n");
                }
                memcpy(pwd2,pathname,strlen(pathname));//将文件名传给 pwd2
                //printf("%s\n",pwd2);
                stat(pwd2,&st);//将 path 的文件属性赋值 给st

                if(S_ISREG(st.st_mode))//如果是普通文件
                {
                    if(form(pathname,FindFileStyle))
                    {
                        InQueue(qu,(MyStackType)pwd2);
                    }
                }
                else//如果是 文件夹
                {
                    InStack(stack,(MyStackType)pwd2);
                } 
            }

        }
        
        closedir(dir1);//关闭目录
        
    }

    Node * ptr = qu ->front;
    for(int i = 0 ; i < qu ->num ; i ++)
    {
        char * buf = (char *) ptr ->data;
        printf("%s\n",buf);
        ptr = ptr ->next;
    }
    printf("qu -> num = %d\n",qu ->num);


    return qu;
}


