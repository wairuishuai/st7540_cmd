#include "bit_define.h"
#include "power_line.h"
#include "iostm8s103.h"
const unsigned char next_state_table[] =
{
	0, 1, 1, 0, 2, 1, 3, 2
};

unsigned char curr_head = 0;
unsigned char frame_head_found = 0;
unsigned char pwl_data_catch = 0;
unsigned char pwl_bit_cnt = 0;
unsigned char pwl_tx_wking = 0;
unsigned char pkt_end_flag  = 0 ;

PWR_LINE_TRANS_PACK pwr_line_tx_pack ;
PWR_LINE_TRANS_PACK pwr_line_rx_pack ;

unsigned char pwr_line_tx_count=0;

void delay(volatile unsigned long delay_time)   //delay(1000) is about 2.28ms at 16MHz
{
	while(delay_time--) ;
}

void power_line_init(void)
{
	unsigned long reg_value ;

  //reg_value = read_ctrl_reg();
  reg_value = 0x1362d7;
//   	reg_value |= 0x40 ;          //set watch dog
  reg_value &= 0xffffffbf ;        //clr watch dog
  reg_value &= 0xffffbfff;         //sel sync mode
 	reg_value |= 0x18 ;              //set baud rate to 4800
	reg_value &= 0xfffe7fff;         //set MCLK to 16MHz
	reg_value &= 0xfffffffe;         //set MCLK to 110KHz
	reg_value |= 0x00800000;         //enable input filter
  
	write_ctrl_reg(reg_value);
  
	while(reg_value != read_ctrl_reg())
	{
		 write_ctrl_reg(reg_value);
		 delay(500);
	}
	
	PC_CR2 |= BIT6;//ÓÐÖÐ¶Ï
	EXTI_CR1 &= ~BIT5 ;
	EXTI_CR1 |=  BIT4 ;
	
	
}

unsigned long read_ctrl_reg(void)
{
	unsigned long return_value = 0;
	unsigned long curr_bit = 0 ;
    int i ;
    set_pin_rxtx(0x1);     //set pin rxtx
    wait_posedge(); //find posedge for Tcr
    _asm("NOP");
		_asm("NOP");
   // delay(100) ;
    set_pin_reg_data(0x1);  //set pin reg_data
    set_pin_rxtx(0x1);     //set pin rxtx
    
	for(i=0;i<=23;i++)
	{
		wait_posedge();

		curr_bit = get_pin_rxd() ;

		return_value |= ((curr_bit) << (23-i)) ;

	}
	//wait_negedge();

	set_pin_reg_data(0x0);   //clr pin reg_data

	delay(1000) ;
	return return_value ;
}

void write_ctrl_reg(unsigned long write_data)
{
    int i ;

    wait_posedge();   //find posedge for Tcr
    _asm("NOP");
		_asm("NOP");
    //delay(100) ;
    set_pin_reg_data(0x1);   //set pin reg_data
    set_pin_rxtx(0x0);   //clr pin rxtx
  //  wait_negedge();
	//delay(50) ;       //delay for Tcc

	for(i=0;i<=23;i++) //low code effciency may result in wrong writing
	{
		if(((write_data >> (23-i)) & 0x1) == 0)
		{
			set_pin_txd(0x0);
		}
		else
		{
			set_pin_txd(0x1);
		}

		wait_posedge();
		//wait_negedge();
//		delay(32) ;                  //give enough hold time
	}

	set_pin_rxtx(0x1);    //set pin rxtx
	set_pin_reg_data(0x0);   //clr pin reg_data
	set_pin_txd(0x0);
	delay(1000) ;
}

unsigned char data_trans(PWR_LINE_TRANS_PACK *p)
{
  unsigned long tx_data_real ;
	int i ;
	PC_CR2 &= ~BIT6; //disable ext interrupt
	pwl_tx_wking = 1 ;	
	curr_head = 0;  //clear recv state
	frame_head_found = 0; 
	tx_data_real = ((unsigned long *)p)[0];

	wait_posedge();    //find posedge for Tcr	
  //delay(100) ;
  set_pin_reg_data(0x0);   //clr pin reg_data
  set_pin_rxtx(0x0);  //clr pin rxtx
	//wait_negedge();
  //_asm("NOP");
	//delay(10) ;       //delay for Tcc
  
	for(i=0;i<=7;i++) //low code effciency may result in wrong writing
	{
		set_pin_txd(0x1);
		wait_negedge();		
	  set_pin_txd(0x0);
		wait_negedge();		
	}
	
	for(i=0;i<=31;i++) //low code effciency may result in wrong writing
	{
		if(((tx_data_real >> (31-i)) & 0x1) == 0)
		{
			set_pin_txd(0x0);
		}
		else
		{
			set_pin_txd(0x1);
		}
		wait_negedge();			
		//delay(10) ;                  //give enough hold time
	}

  tx_data_real = ((unsigned long *)p)[1]; ;
	for(i=0;i<=31;i++) //low code effciency may result in wrong writing
	{
		if(((tx_data_real >> (31-i)) & 0x1) == 0)
		{
			set_pin_txd(0x0);
		}
		else
		{
			set_pin_txd(0x1);
		}
		wait_negedge();		
		//delay(10) ;                  //give enough hold time
	}
  //wait_negedge();	
	set_pin_rxtx(0x1);   //set pin rxtx
	set_pin_reg_data(0x0);   //clr pin reg_data
	set_pin_txd(0x0);		
	pwl_tx_wking = 0 ;
	wait_posedge();	
	PC_CR2 |= BIT6; //enable ext interrupt	
	return 0 ;
	//while(get_pin_cdpd() == 0x0);
}

unsigned char data_recv(unsigned char curr_bit_o)
{
	unsigned long curr_bit = 0 ;
	curr_bit = ((unsigned long)curr_bit_o) & 0xff;
	pwr_line_rx_pack.aa = FRAME_HEAD ;
	
  if(pwl_bit_cnt <= 23)
	{
		//curr_bit = get_pin_rxd() ;
    ((unsigned long *)&pwr_line_rx_pack)[0] |= ((curr_bit) << (23-pwl_bit_cnt)) ;
	}
	else
  {
		//curr_bit = get_pin_rxd() ;
    ((unsigned long *)&pwr_line_rx_pack)[1] |= ((curr_bit) << (31-(pwl_bit_cnt-24))) ;
	}
	if(pwl_bit_cnt>=55)
	{
		pwl_data_catch = 1;
		pkt_end_flag = 1 ;
		frame_head_found = 0;    
		pwl_bit_cnt = 0 ;
	}
	else
	{
		pwl_bit_cnt++;
	}
		
	return 0 ;
}

unsigned char frame_head_srch(unsigned char curr_bit)
{
	unsigned char next_state = 0;
  curr_head = curr_head << 1;
	if(curr_bit == 0)
	{
		curr_head &= ~1 ;
	}
	else
	{
		curr_head |= 1;
	}

	if(curr_head == FRAME_HEAD)
	{
		curr_head = 0 ;
		return 1;
	}
	else ;

	return 0 ;
}

/*
unsigned char frame_head_srch(unsigned char curr_bit)
{
	unsigned char next_state = 0;

		wait_posedge();
		if(curr_bit ==  ((FRAME_HEAD >> (7-curr_head)) & 0x1))
		{
			if(curr_head == 7)
			{
				curr_head = 0 ;
				return 1 ;
			}
			else ;
			next_state = curr_head + 1;
		}
		else
		{
			next_state = next_state_table[curr_head];
		}

		curr_head = next_state ;

	return 0 ;
}
*/

unsigned char conti_0_check(int conti_0_num)
{
	while(conti_0_num--)
	{
		wait_posedge();
		if(get_pin_rxd() !=  0x0)
		{
			return 0 ;
		}
		else ;
	}

	return 1 ;
}

void wait_posedge(void)
{
	if((PC_IDR & BIT6) != 0)
	{
	//  	_asm("NOP");
  //		_asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
			while((PC_IDR & BIT6) != 0) ;
	}
	else ;
	//    _asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
	while((PC_IDR & BIT6) == 0) ;
	//   _asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
}

void wait_negedge(void)
{
	if((PC_IDR & BIT6) == 0)
	{
	//  	_asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
			while((PC_IDR & BIT6) == 0) ;
	}	
	else ;
  //    _asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
	while((PC_IDR & BIT6) != 0) ;
	//    _asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
	//		_asm("NOP");
}

unsigned char get_pin_rxd(void)
{
	if((PB_IDR & BIT4) == 0)  //get rxd
	{
		return 0x0;
	}
	else
	{
		return 0x1;
	}
}

void set_pin_txd(unsigned char set_to_val)
{
	if(set_to_val == 0x0)
	{
		PC_ODR &= ~BIT4;
	}
	else
	{
		PC_ODR |= BIT4;
	}
}

void set_pin_reg_data(unsigned char set_to_val)
{
	if(set_to_val == 0x0)
	{
		PB_ODR &= ~BIT5;    //clr pin reg_data
	}
	else
	{
		PB_ODR |= BIT5;   //set pin reg_data
	}
}

void set_pin_rxtx(unsigned char set_to_val)
{
	if(set_to_val == 0x0)
	{
		PC_ODR &= ~BIT3;   //clr pin rxtx
	}
	else
	{
		PC_ODR |= BIT3;   //set pin rxtx
	}
}

unsigned char get_pin_cdpd(void)
{
	if((PA_IDR & BIT3) == 0)  //get cdpd
	{
		return 0x0;
	}
	else
	{
		return 0x1;
	}
}

