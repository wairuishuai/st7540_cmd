/*
�ļ�����cmd.c
���ߣ�������ͨ��ѧ ˼Դ0702 ����
ʱ�䣺2013-3-9
���ܣ�

�˻����������п⣬����ʹ�ó����ն˽���������ͨ�š�
V2.0�汾���빦�ܣ�
1��echo off���ܣ��ɹرջ���
2��ʵ�ֿ��ŷ��ԭ��ÿ��Ӧ��ָ��ֲ�д�������ļ��У�ʹ��ʱ����ע�ᣬ�����޸Ĵ��ļ�

*/
#include "cmd.h"

#define MAX_BUFFSIZE	40

const char *HelloString="CMD-Lib V2.0\r\nBJTU-SY0702-YangZi\r\n>";


Queue cmd_queue;
static u8 cmd_queue_buff[10];

static CMD_LIST *cmd_list=0;

void fun_help(s8 *para);
void fun_cpu(s8 *para);
void fun_echo(s8 *para);


const CMD_STR cmd_str[]=\
{
	(s8 *)"?",(s8 *)"����",fun_help,
	(s8 *)"echo",(s8 *)"����1,0",fun_echo,
	(s8 *)"CPU",(s8 *)"CPUռ��",fun_cpu,	//CPUռ����
};
static CMD_LIST sys_cmd_list={cmd_str,sizeof(cmd_str)/sizeof(CMD_STR),0};
void cmd_ini(void)
{
	Queue_ini(cmd_queue_buff,sizeof(cmd_queue_buff),&cmd_queue);
	cmd_reg((CMD_LIST*)&sys_cmd_list);
	printf(HelloString);
}

void cmd_reg(CMD_LIST *a)
{
	a->next=cmd_list;
	cmd_list=a;
}

s8 cmd_c=0;		//��ǰָ��
u8 buff_p=0;	//��ǰָ���ַ�����ĩβ��ָ��\0��
u8 cmd_para=0;	//����ƫ��
s8 cmd_buff[MAX_BUFFSIZE]={0};	//�������ַ�����

void (*cmd)(void)=cmd_fun;	//������

void shell(void);

void cmd_fun(void)	//������
{
	//while(!queue_get(&cmd_c))	//����0˵��������ֵ
	while(Queue_get(&cmd_c,&cmd_queue)==0)
	{
	//�������
		if(cmd_c=='\n' || cmd_c=='\r')	//���ǻس�
		{
			printf("\r\n");	//����
			cmd_buff[buff_p++]='\0';
			//����
			shell();
			//������ɣ���Ҫ��ָ�����
			buff_p=0;
			cmd_para=0;
			printf("\r\n> ");	//��ʾ��
		}
		else if(cmd_c==0x08)	//�˸��
		{
			printf("%c %c",cmd_c,cmd_c);	//����
			buff_p--;
		}
		else		//������ͨ�ַ�
		{
			cmd_buff[buff_p++]=cmd_c;
			printf("%c",cmd_c);	//����
		}
		if(buff_p>=MAX_BUFFSIZE-1)
		{
			buff_p=MAX_BUFFSIZE-2;
		}
	}
}
void cmd_fun_echooff(void)	//������,�޻���
{
	//while(!queue_get(&cmd_c))	//����0˵��������ֵ
	while(Queue_get(&cmd_c,&cmd_queue)==0)
	{
	//�������
		if(cmd_c=='\n' || cmd_c=='\r')	//���ǻس�
		{
			//printf("\r\n");	//����
			cmd_buff[buff_p++]='\0';
			//����
			shell();
			//������ɣ���Ҫ��ָ�����
			buff_p=0;
			cmd_para=0;
			//printf("\r\n> ");	//��ʾ��
		}
		else if(cmd_c==0x08)	//�˸��
		{
			//printf("%c %c",cmd_c,cmd_c);	//����
			buff_p--;
		}
		else		//������ͨ�ַ�
		{
			cmd_buff[buff_p++]=cmd_c;
			//printf("%c",cmd_c);	//����
		}
		if(buff_p>=MAX_BUFFSIZE-1)
		{
			buff_p=MAX_BUFFSIZE-2;
		}
	}
}

void shell(void)
{
//��ȡָ��������ָ���Ӧ�ķ��������
	s8 *str,i;
	CMD_LIST *tmp=cmd_list;
	for(i=0;i<buff_p;i++)
	{
		if(cmd_buff[i]!=' ')
		{
			str=&cmd_buff[i];
			for(;i<buff_p;i++)
			{
				if(cmd_buff[i]==' ')
				{
					break ;
				}
			}
			break ;
		}
	}
	cmd_buff[i]='\0';
	if(i==buff_p)	//˵��û�в���
	{
		cmd_para=0;
	}
	else
	{
		cmd_para=i+1;
	}
//�õ��������ַ�str,�Ͳ���ƫ��
	while(tmp)
	{
		for(i=0;i<tmp->num;i++)
		{
			if(strcmp((char const *)tmp->str[i].str,(char const *)str)==0)
			{
				//ִ����Ӧ����ķ�����
				if(cmd_para)	//�в���
				{
					tmp->str[i].fun(cmd_buff+cmd_para);
				}
				else	//�޲���
				{
					tmp->str[i].fun((s8 *)"");
				}
				return ;
			}
		}
		tmp=tmp->next;
	}
	if(str[0]!=0)
	{
		printf("bad command");
	}
}

////////////////////////////////////////////////////////////////////////
//������
void fun_help(s8 *para)
{
	int i;
	CMD_LIST *tmp=cmd_list;
	//���Ƚ�����ģʽ��Ϊ�л���
	//cmd=cmd_fun;
	while(tmp)
	{
		for(i=0;i<tmp->num;i++)
		{
			printf("%s\t%s\r\n",tmp->str[i].str,tmp->str[i].des);
		}
		tmp=tmp->next;
	}
}
void fun_cpu(s8 *para)
{
	printf("%d",SYS_idle_dis);
}
void fun_echo(s8 *para)
{
	u32 t1;
	if(sscanf(para,"%d",&t1)==1)
	{
		if(t1)
		{
			cmd=cmd_fun;
		}
		else
		{
			cmd=cmd_fun_echooff;
		}
	}
}