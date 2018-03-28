/*
�ļ�����uart.c
���ߣ�������ͨ��ѧ ˼Դ0702 ����
ʱ�䣺2012-8-7
���ܣ�

*/
#include "uart.h"
#include "queue.h"
#include "power_line.h"

Queue uart_rx_queue;
Queue uart_tx_queue;
static u8 uart_rx_queue_buff[90];
static u8 uart_tx_queue_buff[90];

u8 uart_busy = 0; //0Ϊ����
void uart_ini(u32 b)
{
	//��ʼ�����ţ�
	PD_DDR &= ~(1 << 6); //���루�����
	PD_CR1 |= (1 << 6); //���������գ�
	PD_CR2 &= ~(1 << 6); //���жϣ��жϣ�

	PD_DDR |= (1 << 5); //��������룩
	PD_CR1 |= (1 << 5); //���죨��©��
	PD_CR2 &= ~(1 << 5); //���٣����٣�
	//��ʼ��uart
	b = 16e6 / b; //8M�ⲿʱ�� 8e6 is 8000000
	UART1_BRR2 = ((b & 0xf000) >> 8) + (b & 0x000f);
	UART1_BRR1 = ((b & 0x0ff0) >> 4);
	//UART2_CR1=0;
	//UART2_CR2=0x6c;//01101100//׼�����ж�
	UART1_CR2 = 0x2c; //00101100//��ֹ�����ж�
	//UART2_CR3=0;
	//UART2_CR4=
	uart_busy = 0;
	UART1_SR = 0;

	Queue_ini(uart_rx_queue_buff, sizeof(uart_rx_queue_buff), &uart_rx_queue);
	Queue_ini(uart_tx_queue_buff, sizeof(uart_tx_queue_buff), &uart_tx_queue);
}

char putchar(char c)
{
	UART1_DR=c;
	while(!(UART1_SR & 0x80));
	return c;
}

void data_to_uart(void)//����ģʽ�µĴ��ڷ��ͺ���
{
	s8 c;
	if(UART1_SR & 0x80)
	{
	  if(Queue_get(&c, &uart_tx_queue) == 0)
	  {
		  //UART1_SR &= ~0x40;
	    UART1_DR=c;
	  }
  }
	else ;
}

@far @interrupt void uart1_int(void)
{
	if(UART1_SR & 0x20)//�����ж�
	{
			pwr_line_tx_count++;
			Queue_set(UART1_DR, &uart_rx_queue);
			if(pwr_line_tx_count>=6)
			{
				START_TASK(TASK_PWRLINE_TX);
			}
	}
	UART1_SR &= ~0x20;	
}
