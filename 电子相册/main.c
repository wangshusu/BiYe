#include "getpath.h"
#include "showphoto.h"
#include "touch.h"

int main(int argc ,char *argv[])
{
    if(argc < 2)
    {
        printf("input error\n");
        return -1;
    }
    Queue * qu = get_dir_size(argv[1],".bmp");

    Node *ptr = qu ->front;
    showphoto((char *)ptr ->data,0,0,1);

    while (1)
    {
        int cmd = GetMove_Dir(ptr);
        if(cmd != DIR_ERROR)
        {
            if(cmd == 0 || cmd == 3)//右滑 下滑 下一张
            {
                ptr = ptr ->next;
                showphoto((char *)ptr ->data,0,0,1);
                continue;
            }
            else if(cmd == 1 || cmd == 2)//左滑 上滑 上一张
            {
                ptr = ptr ->prev;
                showphoto((char *)ptr ->data,0,0,1);
                continue;
            }
        }
    }
    
}
    //0 RIGHT,//右滑
    //1 LIFT,//左滑
    //2 UP,//上滑
    //3 DOWN,//下滑
