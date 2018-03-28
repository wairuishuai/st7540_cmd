#define FRAME_HEAD  0x9b

typedef struct
{
	unsigned char aa;//同步字
	unsigned char data_num;//数据数量
	unsigned char data[6];
} PWR_LINE_TRANS_PACK;//数据包模式传输结构

extern unsigned char pwr_line_tx_count;
extern PWR_LINE_TRANS_PACK pwr_line_tx_pack ;
extern PWR_LINE_TRANS_PACK pwr_line_rx_pack ;

void power_line_init(void);
void delay(volatile unsigned long  delay_time);   //delay(1000) is about 2.28ms at 16MHz
unsigned long read_ctrl_reg(void);
void write_ctrl_reg(unsigned long write_data);
unsigned char data_trans(PWR_LINE_TRANS_PACK *p);
//unsigned char data_recv(void);
//unsigned char frame_head_srch(void);
unsigned char data_recv(unsigned char curr_bit_o);
unsigned char frame_head_srch(unsigned char curr_bit);
unsigned char get_pin_rxd(void);
void set_pin_txd(unsigned char set_to_val);
void set_pin_reg_data(unsigned char set_to_val);
void set_pin_rxtx(unsigned char set_to_val);
void wait_posedge(void);
void wait_negedge(void);
unsigned char get_pin_cdpd(void);
unsigned char conti_0_check(int conti_0_num);

extern unsigned char curr_state;
extern unsigned char frame_head_found;
extern unsigned char pwl_data_catch;
extern unsigned char pwl_bit_cnt ;
extern unsigned char pwl_tx_wking ;
extern unsigned char pkt_end_flag ;