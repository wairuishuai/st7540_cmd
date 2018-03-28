/*
文件名：main.c
作者：北交大 自控1102 杨孜
时间：2012-06-29

GPIO 实例：

	PD_DDR &= ~(1<<6);//输入（输出）
	PD_CR1 |= (1<<6);//上拉（浮空）
	PD_CR2 &= ~(1<<6);//无中断（中断）

	PD_DDR |= (1<<5);//输出（输入）
	PD_CR1 |= (1<<5);//推挽（开漏）
	PD_CR2 &= ~(1<<5);//低速（高速）

*/
#include "main.h"
#include "queue.h"
#include "uart.h"

#include "bit_define.h"
#include "power_line.h"

u16 sys_time = 0;
u8 SYS_task = 0;
extern unsigned char curr_head;

unsigned char pkt_interval_cnt = 0 ;

void CLK_ini(void)
{
	*(u8*)0x4807 |= (1<<3);
//	CLK_ECKCR=1;//打开外部振荡器
	CLK_SWR=0xb4;//切换为外部震荡;E1:HSI;D2:LSI
	CLK_SWCR|=2;//开始切换
	CLK_CKDIVR=0x0;//不分频
	
//	CLK_PCKENR1=0x18;//低功耗，关闭不用的设备
//	CLK_PCKENR2=0x08;
}

void TIM4_ini(void)
{
	TIM4_IER = 0;//禁止中断
	TIM4_EGR = 1;//允许产生更新事件
	TIM4_PSCR = 7; //分频
	TIM4_ARR = 255;
	TIM4_CNTR = 255;
	TIM4_CR1 = 0x01; //允许计数工作
	TIM4_IER = 1;//禁止中断
	//TIM4_SR  =
	//TIM4_CNTR=
	TIM4_EGR = 1;
}

void GPIO_init(void)
{
//cd/pd	
	PA_DDR &= ~BIT3;//输入
	PA_CR1 &= ~BIT3;//float
	PA_CR2 &= ~BIT3;//无中断
//rxd	
	PB_DDR &= ~BIT4;//输入
	PB_CR1 &= ~BIT4;//float
	PB_CR2 &= ~BIT4;//无中断
//reg/data	
	PB_DDR |= BIT5;//输出
	PB_CR1 |= BIT5;//推挽
	PB_CR2 &= ~BIT5;//低速
	PB_ODR &= ~BIT5;//输出
	
//rx/tx
	PC_DDR |= BIT3;//输出
	PC_CR1 |= BIT3;//推挽
	PC_CR2 &= ~BIT3;//低速
    PC_ODR |= BIT3;//输出
//txd
	PC_DDR |= BIT4;//输出
	PC_CR1 |= BIT4;//推挽
	PC_CR2 &= ~BIT4;//低速
	PC_ODR &= ~BIT4;//输出
//bu/theam
	PC_DDR &= ~BIT5;//输入
	PC_CR1 &= ~BIT5;//float
	PC_CR2 &= ~BIT5;//无中断
//clr/t
	PC_DDR &= ~BIT6;//输入
	PC_CR1 &= ~BIT6;//float
	PC_CR2 &= ~BIT6;//无中断
//rsto
	PC_DDR &= ~BIT7;//输入
	PC_CR1 &= ~BIT7;//float
	PC_CR2 &= ~BIT7;//无中断
//adcin
	PD_DDR &= ~BIT3;//输入
	PD_CR1 &= ~BIT3;//float
	PD_CR2 &= ~BIT3;//无中断
  //PD_ADC_TDR |= BIT3;//ADC
	
//	PD_DDR |= BIT3;//输出
//	PD_CR1 |= BIT3;//推挽
//	PD_CR2 |= BIT3;//低速
//  PD_ODR &= ~BIT3;
	
	
	
//uart/spi	
	PD_DDR |= BIT2;//输出
	PD_CR1 |= BIT2;//推挽
	PD_CR2 &= ~BIT2;//低速
	PD_ODR &= ~BIT2;
//wd		
	PD_DDR |= BIT4;//输出
	PD_CR1 |= BIT4;//推挽
	PD_CR2 &= ~BIT4;//低速
    PD_ODR &= ~BIT4;

	//while((PC_IDR & BIT6) == 0x0) ; //wait rtso == 1
	delay(1000) ;
	PD_ODR |= BIT4;    //watch dog
	delay(100) ;
	PD_ODR &= ~BIT4;

}

Queue *can_tx=&uart_rx_queue;
void IWDG_Init(void)
{
    IWDG_KR = 0xCC; //启动IWDG
	IWDG_KR = 0x55; //解除 PR 及 RLR 的写保护
	IWDG_RLR = 0xff; //看门狗计数器重装载数值       
	IWDG_PR = 0x07; //分频系数为256，最长超时时间为：1.02S,0为4分频
	IWDG_KR = 0xAA; //刷新IDDG，避免产生看门狗复位，同时恢复 PR 及 RLR 的写保护状态
}
main()
{
	int i ;
    delay(10000) ;
	GPIO_init();
	power_line_init();
	CLK_ini();
	uart_ini(4800);//默认串口配置
	TIM4_ini();

	EnableInt;
	//IWDG_Init();
	while (1)
	{
		//主循环高速轮询串口发送任务，只在传输模式用，配置模式用putchar
			data_to_uart();
		//CAN发送任务,定时触发，满8字节触发
			if(SYS_task & TASK_PWRLINE_TX)
			{
				s8 t;
				u8 n;
				SYS_task &= ~TASK_PWRLINE_TX;
				if(pwr_line_tx_pack.data_num)//若有待发送数据
				{
					t=data_trans(&pwr_line_tx_pack);
					if(t==0)//若发出去了
					{
						pwr_line_tx_pack.data_num=0;
						pwr_line_tx_count=0;
						for(i=0;i<6;i++)
						{
							pwr_line_tx_pack.data[i] = 0 ;
						}
						//delay(10000);						
					}
					//delay(50000);
					//发出去了要取下一组值，
					//没发出去要立刻查看
					START_TASK(TASK_PWRLINE_TX);
				}
				else//若没有待发送数据，则读队列
				{
						n=0;
						while(Queue_get(&t, &uart_rx_queue)==0)
						{
							pwr_line_tx_pack.aa = FRAME_HEAD ;
							pwr_line_tx_pack.data[n]=t;
							n++;
							if(n>=6)
							{
								break;
							}
						}
						pwr_line_tx_pack.data_num=n;
						if(n!=0)//若没有数据就不用了
						{
							START_TASK(TASK_PWRLINE_TX);
						}
				}
			}

      if(1 == pwl_data_catch)
			{
					pwl_data_catch = 0;
					if(pwr_line_rx_pack.data_num > 6)
					{
						pwr_line_rx_pack.data_num = 6 ;
					}
					for(i = 0; i < pwr_line_rx_pack.data_num; i++)
					{
						Queue_set((s8)(pwr_line_rx_pack.data[i]), &uart_tx_queue);
					}
					
					((unsigned long *)&pwr_line_rx_pack)[0] = 0;
					((unsigned long *)&pwr_line_rx_pack)[1] = 0;
										
			}
		//喂狗
		//IWDG_KR=0XAA;
	}
}
void wait(vu32 i)
{
	while(i--);
}

@far @interrupt void portc_int(void)
{
	unsigned char curr_bit = 0 ;
	curr_bit = get_pin_rxd() ;
	
	if(pkt_end_flag == 1)
	{
			if(pkt_interval_cnt >= 16)
			{
					pkt_end_flag = 0 ;
					pkt_interval_cnt = 0 ;
		  }
			else
			{
					pkt_interval_cnt ++ ;
		  }			
			return ;
	}

	if((get_pin_cdpd() != 0) && (frame_head_found == 0))
	{
		curr_head = 0;
		return ;
	}
	else ;	

	if(pwl_tx_wking == 0)
	{
	  if(frame_head_found == 0)
	  {
	    if(frame_head_srch(curr_bit) !=0)
	    {
		  	frame_head_found = 1 ;
	    }
	    else ;	
    }
	  else
	  {
	  	data_recv(curr_bit);
	  }
	}
	else ;
}

@far @interrupt void Tim4_int(void)//
{
	TIM4_SR = 0;
	//STimer_int();
	START_TASK(TASK_PWRLINE_TX);
}
@far @interrupt void trap_int(void)//软中断
{

}
