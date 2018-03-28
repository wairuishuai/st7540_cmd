/*
文件名：queue.c
作者：北京交通大学 思源0702 杨孜
时间：2011-1-22

功能：

*/

#include "queue.h"

void Queue_ini(u8 *d, u16 num, Queue *q)
{
	q->start = 0;
	q->end = 0;
	q->len = num;
	q->isFull = 0;	//空
	q->q_data = d;
	q->count=0;
}

s8 Queue_get(s8 *data, Queue *q)
{
	if(q->end == q->start)	//要么队列满，要么空
	{
		if(q->isFull == 0)	//若队列空
		{
			return 1;
		}
		else	//队列满
		{
			q->isFull = 0;
		}
	}
	*data = q->q_data[q->start++];
	if(q->start >= q->len)
	{
		q->start = 0;
	}
	q->count--;
	return 0;
}

s8 Queue_set(s8 data, Queue *q)
{
	if(q->isFull == 1)	//若队列满了
	{
		return 1;
	}
	q->q_data[q->end++] = data;
	if(q->end >= q->len)
	{
		q->end = 0;
	}
	if(q->start == q->end)	//说明队列恰好满了
	{
		q->isFull = 1;
	}
	q->count++;
	return 0;
}
