/*
�ļ�����queue.h
���ߣ�������ͨ��ѧ ˼Դ0702 ����
ʱ�䣺2011-1-22

���ܣ�

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
	u8 isFull;	//Ϊ1��־������,Ϊ0��ʾû����������ʱ���ǿգ�start��endһ������
	u16 count;//�����е�����
} Queue;

void Queue_ini(u8 *d, u16 num, Queue *q);

//�ɹ�����0��ʧ�ܷ��ش����
s8 Queue_set(s8 data, Queue *q);
s8 Queue_get(s8 *data, Queue *q);


#endif