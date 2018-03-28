/*
文件名：uart.h
作者：北京交通大学 思源0702 杨孜
时间：2012-8-7
功能：

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
