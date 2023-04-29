#ifndef __LINKEDLISTQUEUE_H__
#define __LINKEDLISTQUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef void* MyStackType;

#define QueueMaxnum 10240

typedef struct Node
{
	MyStackType data;
	struct Node * next;
	struct Node * prev;   //需要双向链表
	
}Node;

typedef struct
{
	struct Node * rear;
	struct Node * front;
	int num;
}Queue;

/*
	初始化
	返回一个队列头
*/
Queue *InitQueue(void);

/*
	销毁这个队列 传二级指针进来 改变那边的指向
	用户自己指定处理规则 void callback(void * data)
	回调函数可以指定NULL 表示没有规则不处理这个数据
*/	
void DestroyQueue(Queue **qu,void (*callback)(MyStackType));

/*
	清空这个队列
	用户自己指定处理规则 void callback(void * data)
	回调函数可以指定NULL 没有规则默认 free(data)
*/
void ClearQueue(Queue *qu,void (*callback)(MyStackType));

/*
	判断这个队列是否是空的
	为空返回:true
	不为反回:false
*/	
bool QueueIsEmpty(Queue *qu);

	
/*
	返回队头元素(不出队)
	没有队列返回 NULL
*/
MyStackType QueueGetTop(Queue *qu);

/*
	出队
	没有队列返回 NULL
*/
MyStackType OutQueue(Queue *qu);

/*
	入队
	没有队列返回 NULL
*/	
int InQueue(Queue *qu,MyStackType data);

#endif
