/*
�ļ�����uart.h
���ߣ�������ͨ��ѧ ˼Դ0702 ����
ʱ�䣺2012-8-7
���ܣ�

*/
#ifndef UART_H
#define UART_H

#include "main.h"
#include "queue.h"

void uart_ini(u32 b);

void data_to_uart(void);

extern Queue uart_rx_queue;
extern Queue uart_tx_queue;

#endif
