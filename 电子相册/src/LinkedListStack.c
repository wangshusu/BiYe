#include "LinkedListStack.h"

/*
    初始化函数 返回一个栈
*/
Stack *InitStack(void)
{
    Stack *pnew = calloc(1,sizeof(pnew));
    if(!pnew)
        return NULL;
    pnew ->front = pnew ->rear = NULL;
    pnew ->num = 0;
    return pnew;
}

/*
    进栈
*/
void InStack(Stack *st,MyStackType data)
{
    if(!st)
        return;
    if(StackMax < st ->num)//判断是否超过了最大的
    {
        printf("Too Much!!\n");
        return;
    }
    //先用东西来接住这个值
    StackNode *pnew = calloc(1,sizeof(pnew));
    pnew ->data = data;
    pnew ->next = NULL;

    //如果里面没有数据
    if(0 == st ->num)
    {   //那么这个就是第一个节点
        st ->front = st ->rear = pnew;
        st ->num++;//添加了节点 这里的数字就要加一
        return;
    }
    //如果不是第一个  那么这里就是用头插法
    pnew ->next = st ->front;
    st ->front = pnew;
    st ->num++;
}

/*
    出栈,出栈有返回值,返回出栈元素存储的data
*/
MyStackType OutStack(Stack *st)
{
    if(!st )
        return st;
    if(0 == st ->num)
    {
        printf("The Stack was NULL!\n");
        return st;
    }
    //先把这个值保存起来
    MyStackType data1 = st ->front ->data;
    StackNode *ptr = st ->front;
    //如果是最后一个还要出队
    if(0 == st ->num)
    {
        st ->front = st ->rear = NULL;
        st ->num--;
        free(ptr);
        return data1;
    }
    //如果不是最后一个
    st ->front = st ->front ->next;
    free(ptr);
    st ->num--;
    return data1;
}

/*
    销毁函数,调用清空函数,
    @void (*callback)(MyStackType):传入参数可以对清空的数据进行处理
*/
void DestoryStack(Stack **st,void (*callback)(MyStackType ))
{
    //销毁传的是二级指针 这里要改变他的指向
    ClearStack(*st,callback);
    free(*st);
    *st = NULL;
    printf("DestoryStack Success!!!\n");
}

/*
    清楚栈内的空间,释放 st 在堆区开辟的空间
*/
void ClearStack(Stack *st,void (*callback)(MyStackType ))
{
    if(!st)
        return;
    while(st ->num)
    {
        MyStackType data = OutStack(st);
        if(callback)
        {
            callback(data);
        }
        else
        {
            free(data);
        }
    }
}

/*
    返回栈的长度(return st ->length)
*/
int LengthStack(Stack *st)
{
    return st ->num;
}


/*
    判断栈是否为空
    为空:返回 true
    不空:返回 false
*/
bool StackEmpty(Stack *st)
{
    if(st ->num != 0)
        return false;
    return true;
}

/*
    取栈顶元素
    返回当前的 栈顶 元素,并通过返回值返回
    返回类型为 MyStackType
*/
MyStackType GetTop(Stack *st)
{
    return (MyStackType) st ->front ->data;
}