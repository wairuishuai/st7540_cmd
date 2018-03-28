/*
文件名：main.h
作者：北京交通大学 自控1102 杨孜
更改时间：2012-06-29

*/
#ifndef MAIN_H
#define MAIN_H


#include "iostm8s103.h"
#include "stdio.h"
#include "ctype.h"
#include "string.h"

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;
typedef signed char s8;
typedef signed int s16;
typedef signed long s32;
typedef volatile unsigned char vu8;
typedef volatile unsigned int vu16;
typedef volatile unsigned long vu32;
typedef volatile signed char vs8;
typedef volatile signed int vs16;
typedef volatile signed long vs32;

#define DisableInt	_asm("SIM")
#define EnableInt	_asm("RIM")
#define SYS_SERVICE	_asm("TRAP")
#define LOW_POWER_WAIT	_asm("WFI")

extern u8 SYS_task;

#define START_TASK(x)       SYS_task |= x

#define TASK_PWRLINE_TX	            1	//CAN发送任务

//系统提供的函数:
void wait(vu32 i);		//延迟函数
void void_fun(void);	//空函数
void err(void);			//错误函数（死循环）

#define CHANGE_END16(n) (u16)((((u16) (n)) << 8) | (((u16) (n)) >> 8))
#define CHANGE_END32(n)	(u32)( ((n)<<24) | (((n)&0xff00)<<8) | (((n)&0xff0000)>>8) | (((u32)(n))>>24) )

#endif
