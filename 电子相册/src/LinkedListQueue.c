#include "LinkedListQueue.h"

/*
	初始化
	返回一个队列头
*/
Queue *InitQueue(void)
{
	Queue * qu = malloc(sizeof(Queue));
	qu ->num = 0;
	qu ->rear = qu ->front = NULL;
	return qu;
	
}

/*
	销毁这个队列 传二级指针进来 改变那边的指向
	用户自己指定处理规则 void callback(void * data)
	回调函数可以指定NULL 表示没有规则不处理这个数据
*/					
void DestroyQueue(Queue **qu,void (*callback)(MyStackType))
{
	//清空这个队列
	ClearQueue(*qu,callback);
	//将这个队列释放
	free(*qu);
	//改变指向
	*qu = NULL;
	
}

/*
	清空这个队列
	用户自己指定处理规则 void callback(void * data)
	回调函数可以指定NULL 没有规则默认 free(data)
*/		
void ClearQueue(Queue *qu,void (*callback)(MyStackType))
{
	while(!QueueIsEmpty(qu))//不为空才会需要删除
	{
		void * data = OutQueue(qu);

		if(callback)//传了这个函数指针才去处理
		{
			callback(data);
		}
		else//默认直接free(data)
		{
			free(data);
		}
	}	
}

/*
	判断这个队列是否是空的
	为空返回:true
	不为反回:false
*/		
bool QueueIsEmpty(Queue *qu)
{
	if(!qu || qu -> num == 0)
		return true;
	else
		return false;
	
}

/*
	出队
	没有队列返回 NULL
*/	
MyStackType OutQueue(Queue *qu)
{
	if(!qu || !qu ->front)
		return NULL;
	
	MyStackType data = qu ->front ->data;

	if(qu ->num == 1)//队列里面只有一个元素你还要出队
	{
		free(qu ->front);
		qu ->front = qu ->rear = NULL;
		qu ->num--;		
		return data;
	}

	Node * ptr = qu ->front; 
	qu ->front = qu ->front ->next;
	ptr ->next = NULL;
	qu ->rear ->next = qu ->front;

	free(ptr);
	qu ->num--;
	
	return data;
}

/*
	入队
	没有队列返回 NULL
*/		
int InQueue(Queue *qu,MyStackType data)	
{
	if(!qu)
	{
		return 0;
	}
	if(QueueMaxnum <=  qu ->num)//栈里面的元素太多了  不能加了
	{
		printf("\n栈里面的元素太多了,不能加了\n");
		return 0;
	}
	
	Node * pnew = malloc(sizeof(Node));
	pnew ->data = data;
	pnew ->next = pnew ->prev =NULL;
	
	if(qu ->num == 0)//队列里面没有数据
	{
		qu ->front = qu ->rear = pnew;
		pnew ->prev = pnew ->next = pnew;//双向链表
		qu ->num++;
		return 666;
	}

	//队列本身是存在的 我们就进行尾插
	pnew ->prev = qu ->rear;
	pnew ->next = qu ->front;
	qu ->rear ->next = pnew;
	qu ->front ->prev = pnew;
	qu ->rear = pnew;


	qu ->num++;
	return 666;
}	







