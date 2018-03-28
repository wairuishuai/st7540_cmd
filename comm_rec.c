/*
�ļ�����comm_rec.c
���ߣ�������ͨ��ѧ 
ʱ�䣺2013-9-15
���ܣ�

*/
#include "comm_rec.h"

void rec_sync(u8 b,COMM_SYNC* p)//ͬ����ʽ���պ���
{
	p->rec_buff[p->rec_p++]=b;
	if(b==0x0a||b==0x0d)
	{
		p->rec_buff[p->rec_p]=0;
		//���������ô�����
		p->pro(p->rec_buff,p->rec_p);
		p->rec_p=0;
	}
	if(p->rec_p>=p->buf_len-1)//Ϊ�˼���C����������һ���ֽ�Ҫ��0
	{
		p->rec_p=p->buf_len-2;
	}
}

void rec_head(u8 b,COMM_HEAD* p)
{
	if(p->pre_p<p->syncbuf_len)//����Ѱ�Ұ�ͷ
	{
		if(b==p->SYNC[p->pre_p])//��������ȷ
		{
			p->rec_buff[p->pre_p++]=b;
		}
		else
		{
			p->pre_p=0;
		}
	}
	else if(p->pre_p==p->pre_offset)//ȷ����ͬ���ĳ���
	{
		p->rec_buff[p->pre_p++]=b;
		p->pack_len=p->pre_cb(p->rec_buff,p-> pre_p);
	}
	else//�����������ݰ�
	{
		p->rec_buff[p->pre_p++]=b;
		if(p->pre_p>=p->pack_len)
		{
			//���ô�����
			if(p->pro(p->rec_buff, p->pack_len))
			{//�����ղ���ȷ
				int i;
				int tem_len=p->pack_len;
				p->pre_p=0;

				for( i=p->syncbuf_len;i<tem_len;i++)//�ӽ���ͬ���ֺ�ʼ����
				{
					rec_head(p->rec_buff[i], p);
				}
				return;
			}
			p->pre_p=0;
		}
	}
}

