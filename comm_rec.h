/*
�ļ�����comm_rec.h
���ߣ�������ͨ��ѧ �Կ�1102 ����
ʱ�䣺2013-3-21
���ܣ�

*/
#ifndef COMM_REC_H
#define COMM_REC_H

#include "main.h"
typedef struct
{
	u8 *rec_buff;
	int buf_len;//����������
	int rec_p;
	u8 (*pro)(u8 * b,int len);//�û�������ָ��
} COMM_SYNC;//ͬ�����

void rec_sync(u8 b,COMM_SYNC* p);//ͬ����ʽ���պ���


typedef struct 
{
	u8 *rec_buff;
	int buf_len;//����������
	u8 *SYNC;//ͬ����
	int syncbuf_len;//ͬ���ֳ���
	int pack_len;//��ȫ��
	int pre_offset;//ȷ���������ȵ�λ��
	int pre_p;
	u8 (*pre_cb)(u8 *b,int len);//������������
	u8 (*pro)(u8 * b,int len);//�����Ƿ���ȷ����
}COMM_HEAD;//�������������

void rec_head(u8 b,COMM_HEAD* p);
int pre_cb(u8*b, int len);


#endif
