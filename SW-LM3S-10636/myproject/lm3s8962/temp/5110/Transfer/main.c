#include "include.h"
int value=0;

/*
 * 函数：io_initial
 * 功能：初始化IO
 * 参数：无
 * 返回：无
 */
void io_initial(void)
{
	/*
	 * P1.0：LED
	 * P1.1：IRQ
	 * P1.4：CSN
	 * P1.5：SCLK
	 * P1.6：MISO
	 * P1.7：MOSI
	 * P2.0：CE
	 */
	P1DIR	=	0xef;
	P2DIR	=	0xff;
	P3DIR	=	0xff;
	P1OUT	=	0xfe;
	P2OUT	=	0xff;
	P3OUT	=	0xff;

	P1DIR	|=	BIT0+BIT4+BIT5+BIT7;
	P1DIR	&=	~(BIT1+BIT3+BIT6);
	P2DIR	|=	BIT0;
	P1REN	|=	BIT1+BIT3;
	P1OUT	|=	BIT1+BIT3;
	P1IE	|=	BIT1+BIT3;
	P1IES	|=	BIT1+BIT3;
	P1IFG	&=	~(BIT1+BIT3);
	P1SEL2 = BIT5 | BIT6 | BIT7;
	P1SEL = BIT5 | BIT6 | BIT7;
}

/*
 * 函数：board_initial
 * 功能：初始化时钟、IO、SPI总线等
 * 参数：无
 * 返回：无
 */
void board_initial(void)									//
{

	BCSCTL2 = SELM_0 | DIVM_0 | DIVS_0;
	if (CALBC1_16MHZ != 0xFF)
	{
		DCOCTL = 0x00;
		BCSCTL1 = CALBC1_16MHZ;
		DCOCTL = CALDCO_16MHZ;
	}
	BCSCTL1 |= XT2OFF | DIVA_3;								// ACLK时钟：32768/8=4096Hz
	BCSCTL3 = XT2S_0 | LFXT1S_0 | XCAP_1;

	io_initial();											//配置IO
	USCI_B0_Init();											//初始化硬件SPI，使用UCSIB0
}

int main( void )
{
	unsigned char transfer_value[]="EEWORLD_MSP430_00";//发送数据完整内容

	WDTCTL = WDTPW + WDTHOLD;
	board_initial();
	_EINT();

	Set_Module_TX();//将模块设置为发送模式
	TA1_Init();//初始化定时器A1，每0.5秒唤醒MCU发送数据


	for(;;)
	{
		transfer_value[16]=value&0x00ff;			//将value的低8位填充进数组
		transfer_value[15]=((value&0xff00)>>8);		//将value的高8位填充进数组
		Transfer_String(transfer_value, 17);		//发送数组，发送长度为17字节
		value++;
		LPM3;										//进入LPM3，由外部中断或者定时器中断唤醒。
	}
}


/*
 * 外部中断，由按键和外部IRQ中断触发，共用中断向量
 * P1.3的功能：按下按键，发送数据自增1000
 * P1.1的功能：IRQ中断，说明数据发送完毕
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
	if(P1IFG & BIT3)		//按键中断
	{
		value+=1000;		//按下按键，发送数据自增1000
		P1IFG &= ~BIT3;		//清中断标志位
		LPM3_EXIT;			//退出低功耗模式，在主循环中发送下一帧数据
	}
	if(P1IFG & BIT1)		//IRQ中断，说明一帧数据已经发送完毕
	{
		unsigned char tmp=SPI_Read_Reg(READ_REG+STATUS);	//读取状态寄存器

		P1IFG &= ~BIT1; 									//清除外部中断。
															//等会可能需要用到中断嵌套，开启中断使能
															//如果不关中断，那么中断使能之后将直接再次进入中断服务

		tmp&=0x70;											//检测IRQ中断源
		SPI_Write_Reg(WRITE_REG+STATUS,tmp);				//清除模块IRQ中断
		if(tmp&0x20)										//如果IRQ中断是发送中断
		{

			_EINT();										//中断使能
			P1OUT |= BIT0;									//点亮LED
			Delay_10us(100);								//调用定时器A0延时1ms
			P1OUT &= ~BIT0;									//熄灭LED
		}

		tmp=SPI_Read_Reg(READ_REG+FIFO_STATUS);             //读取FIFO寄存器状态
	    if(!(tmp&0x10))                                     //如果发送FIFO非空，说明尚有数据未发出，需要让CE管脚产生一个高脉冲发送下一帧数据
	    {
	    _EINT();                                            //中断使能
		SET_NRF24L01_CE_H;
	    Delay_10us(1);
	    Delay_10us(1);
	    SET_NRF24L01_CE_L;
	    }
		P1IFG &= ~BIT1;										//清除外部中断标志位
	}
}


