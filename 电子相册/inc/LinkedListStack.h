#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define StackMax 10240
typedef void * MyStackType;
typedef char ElemTpe;

typedef struct StackNode{
    //数据域
    MyStackType data;
    //指针域
    struct StackNode *next;
}StackNode;

typedef struct Stack{

    int num;
    struct StackNode *front;//头部
    struct StackNode *rear ;//尾部

}Stack;

/*
    初始化函数 返回一个栈
*/
Stack *InitStack(void);

/*
    进栈
    栈的最大容量为 StackMax
*/
void InStack(Stack *st,MyStackType data);

/*
    出栈,出栈有返回值,返回出栈元素存储的data
*/
MyStackType OutStack(Stack *st);

/*
    销毁函数,调用清空函数,
    @void (*callback)(MyStackType):传入参数可以对清空的数据进行处理
    不传参数默认 free(data)
*/
void DestoryStack(Stack **st,void (*callback)(MyStackType ));

/*
    清楚栈内的空间,释放 st 在堆区开辟的空间
*/
void ClearStack(Stack *st,void (*callback)(MyStackType ));

/*
    返回栈的长度(return st ->length)
*/
int LengthStack(Stack *st);

/*
    判断栈是否为空
    为空:返回 1
    不空:返回 0
*/
bool StackEmpty(Stack *st);

/*
    取栈顶元素
    返回当前的 栈顶 元素,并通过返回值返回
    返回类型为 MyStackType
*/
MyStackType GetTop(Stack *st);

#endif