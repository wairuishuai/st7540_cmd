/*
�ļ�����queue.c
���ߣ�������ͨ��ѧ ˼Դ0702 ����
ʱ�䣺2011-1-22

���ܣ�

*/

#include "queue.h"

void Queue_ini(u8 *d, u16 num, Queue *q)
{
	q->start = 0;
	q->end = 0;
	q->len = num;
	q->isFull = 0;	//��
	q->q_data = d;
	q->count=0;
}

s8 Queue_get(s8 *data, Queue *q)
{
	if(q->end == q->start)	//Ҫô��������Ҫô��
	{
		if(q->isFull == 0)	//�����п�
		{
			return 1;
		}
		else	//������
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
	if(q->isFull == 1)	//����������
	{
		return 1;
	}
	q->q_data[q->end++] = data;
	if(q->end >= q->len)
	{
		q->end = 0;
	}
	if(q->start == q->end)	//˵������ǡ������
	{
		q->isFull = 1;
	}
	q->count++;
	return 0;
}
