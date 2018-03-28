/*
文件名：queue.h
作者：北京交通大学 思源0702 杨孜
时间：2011-1-22

功能：

*/
#ifndef QUEUE_H
#define QUEUE_H

#include "main.h"

typedef struct
{
	u16 start;
	u16 end;
	u16 len;
	u8 *q_data;
	u8 isFull;	//为1标志队列满,为0表示没有填满，此时若非空，start和end一定不等
	u16 count;//队列中的数量
} Queue;

void Queue_ini(u8 *d, u16 num, Queue *q);

//成功返回0，失败返回错误号
s8 Queue_set(s8 data, Queue *q);
s8 Queue_get(s8 *data, Queue *q);


#endif