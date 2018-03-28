/*
�ļ�����cmd.h
���ߣ�������ͨ��ѧ ˼Դ0702 ����
ʱ�䣺2013-3-9
���ܣ�

ʹ�ð�����

	void fun_a(s8 *para);
	void fun_s(s8 *para);

	static const CMD_STR cmd_str[]=\
	{
		(s8 *)"a",(s8 *)"ȡADֵ",fun_a,
		(s8 *)"s",(s8 *)"IO���",fun_s,
	};
	static CMD_LIST sys_cmd_list={cmd_str,sizeof(cmd_str)/sizeof(CMD_STR),0};

	void con_task_ini()
	{
		cmd_reg((CMD_LIST*)&sys_cmd_list);
	}

*/
#ifndef CMD_H
#define CMD_H

#include "main.h"
#include "queue.h"


typedef struct
{
	s8 *str;	//������
	s8 *des;	//����
	void (*fun)(s8 *para);
} CMD_STR;

typedef struct _tag_CMD_LIST//�����б���Ϊ�����ʼ���Ľṹ
{
	CMD_STR const *str;
	u8 num;//��������ָ�����
	struct _tag_CMD_LIST *next;
} CMD_LIST;

extern const char *HelloString;

extern void (*cmd)(void);	//������
extern Queue cmd_queue;

void cmd_ini(void);
void cmd_reg(CMD_LIST *a);//ע�������б�

void cmd_fun(void);	//���������
void cmd_fun_echooff(void);	//�ⲿͨ��ת������

#endif
