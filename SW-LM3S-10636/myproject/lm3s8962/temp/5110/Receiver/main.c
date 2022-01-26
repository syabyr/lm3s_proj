#include "include.h"
unsigned int Receive_Number;
unsigned char LCD_Leval=0xc0;

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
	P1DIR	=	BIT0+BIT4+BIT5+BIT7;
//	P1DIR	&=	~(BIT1+BIT3+BIT6);
	P1REN	|=	BIT1+BIT3;
	P1OUT	|=	BIT1+BIT3+BIT4;
	P1IE	|=	BIT1+BIT3;
	P1IES	|=	BIT1+BIT3;
	P1IFG	&=	~(BIT1+BIT3);
	P1SEL	=	BIT5 | BIT6 | BIT7;
	P1SEL2	=	BIT5 | BIT6 | BIT7;

	P2DIR	=	BIT0+BIT1+BIT2+BIT3+BIT4+BIT5;
	P2OUT	=	0xff;

	P3DIR	=	0xff;
	P3OUT	=	0xff;
}

/*
 * 函数：board_initial
 * 功能：初始化时钟、IO、LCD、SPI总线等
 * 参数：无
 * 返回：无
 */
void board_initial(void)
{
	if (CALBC1_16MHZ != 0xFF)
	{
		DCOCTL = 0x00;
		BCSCTL1 = CALBC1_16MHZ;
		DCOCTL = CALDCO_16MHZ;
	}
	BCSCTL3 = LFXT1S_2;		//VLO作为ACLK时钟源

	io_initial();           //配置IO
	USCI_B0_Init();			//初始化硬件SPI，使用UCSIB0
}

int main( void )
{
	WDTCTL = WDTPW + WDTHOLD;	//关狗
	board_initial();			//初始化板子
	_EINT();					//开中断
	Set_Module_RX();			//将NRF24L01设置成接收模式
    lcd_init();                 //初始化LCD

	while(1)
	{
		unsigned char transfer_value[]="EEWORLD_MSP430_00";	//定义校验数组
		unsigned char message[20];							//定义接收寄存器数组
		unsigned char i;									//定义一个计数变量i
		LPM3;                                               //进入低功耗模式，由IRQ中断唤醒

/*原来只使用下面这条语句，后发现发送接收存在一个延迟，也就是MCU发往模块的数据某些情况下不能即时到达接收端MCU，而是存在一个延迟，于是将其改成下方的代码，确保接收FIFO的数据被清空*/
//		Get_String(message, 17);							//接收一帧17字节的数据

/*读取FIFO寄存器的状态，当其不为空的时候继续读取FIFO中的数据*/
		i=SPI_Read_Reg(READ_REG+FIFO_STATUS);               //借用i，读取FIFO寄存器状态
        while(!(i&0x01))
        {
            Get_String(message, 17);
            i=SPI_Read_Reg(READ_REG+FIFO_STATUS);               //借用i，读取FIFO寄存器状态
        }
/*上面的语句是为了修正发送延迟的bug新增的，虽然后续实验证明，延迟是发送端造成的，出于谨慎使用模块的角度出发，依旧保留上面的语句*/


        //校验接收到的数据
		for(i=0;i<15;i++)									//用for循环来校验，如果接收到的数组有一个字符与校验数组不一致，那么跳出for循环，此时i会小于15
		{
			if(message[i]!=transfer_value[i])break;
		}
		if(i<15)continue;									//如果i<15，说明校验没有通过，接收数据错误，数值不显示。
		Receive_Number=(message[15]<<8)+message[16];		//将数值从数组message转移到Receive_Number

		//显示接收到的数字
		LCD_Disp_BNum(2,1,Receive_Number/10000%10);
		LCD_Disp_BNum(18,1,Receive_Number/1000%10);
		LCD_Disp_BNum(34,1,Receive_Number/100%10);
		LCD_Disp_BNum(50,1,Receive_Number/10%10);
		LCD_Disp_BNum(66,1,Receive_Number%10);
	}
}
 
/*
 * 外部中断，由按键和外部IRQ中断触发，共用中断向量
 * P1.3的功能：按下按键，LCD亮度(VOP电压值)自增1，VOP电压值从160到230循环
 * P1.1的功能：IRQ中断，说明接收到了一帧数据
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
	if(P1IFG & BIT3)
	{
		P1IFG &= ~BIT3;
		LCD_Leval+=1;
		if(LCD_Leval>250)LCD_Leval=160;
		lcd_init();
		P1IFG &= ~BIT3;
	}
	if(P1IFG & BIT1)
	{
		unsigned char tmp=SPI_Read_Reg(READ_REG+STATUS);	//获取中断状态
		tmp&=0x70;
		SPI_Write_Reg(WRITE_REG+STATUS,tmp);				//配置发送模式
		if(tmp&0x40)
		{
			LPM3_EXIT;
		}
		P1IFG &= ~BIT1;
	}

}
