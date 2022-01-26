/*
---------------------------------------------------------------------------------------------------------
*********************************************************************************************************
** 文件名称 ：	I2CINT.c
** 功能说明 ：	Luminary硬件I2C中断方式软件包。
** 使用说明 ：  主程序要配置好I2C总线接口(I2C引脚功能和I2C中断，并已使能I2C主模式)
*********************************************************************************************************
---------------------------------------------------------------------------------------------------------
*/
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "i2c.h"
#include "interrupt.h"



#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_i2c.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "drivers/rit128x96x4.h"
#include "uart.h"
#include"mydelay.h"
#include <stdint.h>
#include <stdio.h>


#define uchar   unsigned char
#define ulong   unsigned long

/*******************************************************************************************
** 中断中的状态
*******************************************************************************************/ 
#define STATE_IDLE         0					/* 总线空闲状态 		*/
#define STATE_WRITE_ONE    1					/* 写单个字节状态 		*/
#define STATE_WRITE_NEXT   2					/* 写下一个字节状态 	*/
#define STATE_WRITE_FINAL  3					/* 写最后一个字节状态 	*/
#define STATE_READ_ONE     4					/* 读单个字节状态 		*/
#define STATE_READ_FIRST   5					/* 读第一个字节状态 	*/
#define STATE_READ_NEXT    6					/* 读下一个字节状态 	*/
#define STATE_READ_FINAL   7					/* 读最后一个字节状态 	*/
#define STATE_READ_WAIT    8					/* 读字节等待状态 		*/

/*******************************************************************************************
** 全局变量定义
*******************************************************************************************/ 
static volatile uchar 	I2C_sla;				/* I2C器件从地址 					*/
static volatile ulong	I2C_suba;				/* I2C器件内部子地址 				*/
static volatile uchar 	I2C_suba_num;			/* I2C子地址字节数					*/
static volatile uchar 	*I2C_buf;        		/* 数据缓冲区指针  					*/
static volatile ulong   I2C_num;				/* 要读取/写入的数据个数 			*/
static volatile uchar   I2C_opt; 				/* I2c读写操作,0为读操作,1为写操作	*/
static volatile uchar 	I2C_state = STATE_IDLE; /* 中断服务程序的当前状态  			*/


/*
******************************************************************************************
** 函数名称: Delays()
** 函数功能: 延时
** 入口参数: dly
** 出口参数: 无
******************************************************************************************
*/
void Delays (ulong dly)
{
	int i,j;
	for (; dly>0; dly--)
	{
		for (i=0; i<150; i++)
			for (j=0; j<255; j++);
	}
}

/*
*********************************************************************************************************
** 函数名称：I2CInit()
** 函数功能：I2C接口初始化。
** 入口参数：spd		总线速度100K(参数值为100000)或400K(参数值为4000000)
**           pri		中断优先级0～7
** 出口参数：返回值为false时表示操作出错，为true时表示操作正确。
** 说明：
*********************************************************************************************************
*/
int I2CInit(ulong spd, uchar pri)
{
	if ((spd == 400000) || (spd == 100000))
	{
    	// 使能所使用的外设
    	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		
    	// 配置相关引脚，以进行I2C操作
    	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    	 GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_OD);
    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_OD);


    	// 初始化I2C主机,设置主机为低速.
    	if(spd == 400000)
    		I2CMasterInit(I2C_MASTER_BASE, true);
		else
			I2CMasterInit(I2C_MASTER_BASE, false);
		
    	// 使能处理器中断
    	IntMasterEnable();
    	
    	// 使能I2C中断
    	IntEnable(INT_I2C);

    	// 使能I2C主机中断
    	I2CMasterIntEnable(I2C_MASTER_BASE);
    	
    	// 设置I2C中断的优先级
    	IntPrioritySet(INT_I2C, (pri << 5));
    
    	return(true);
    }
    else
    	return(false);
}

/*
*********************************************************************************************************
** 函数名称：ISendByte()
** 函数功能：向无子地址器件发送1字节数据。
** 入口参数：sla		器件地址
**           c			要发送的数据
** 出口参数：返回值为false时表示操作出错，为true时表示操作正确。
** 说明：	使用前要初始化好I2C引脚功能和I2C中断，并已使能I2C主模式
*********************************************************************************************************
*/ 
int ISendByte(uchar sla, uchar c)
{
	I2C_sla = sla >> 1;
	I2C_buf = &c;
	I2C_state = STATE_WRITE_ONE;
	
	I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, false);			// 主机写操作
	I2CMasterDataPut(I2C_MASTER_BASE, *I2C_buf);					// 写一个数据
    I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);	// 启动单次发送
	
	while(I2C_state != STATE_IDLE);
	if(true == I2CMasterBusy(I2C_MASTER_BASE))
		return (false);
	else
		return (true);
}

/*
*********************************************************************************************************
** 函数名称 ：ISendStr()
** 函数功能 ：向有子地址器件任意地址开始写入N字节数据
** 入口参数 ：	sla			器件从地址
**				suba_type	子地址结构	0－8+X结构  1－单字节地址  2－双字节地址  3－三字节地址
**				suba		器件子地址
**				s			数据发送缓冲区指针
**				no			写入的个数
** 出口参数 ：	TRUE		操作成功
**				FALSE		操作失败
*********************************************************************************************************
*/ 
int ISendStr(uchar sla, ulong suba, uchar sub_type, uchar *s, ulong no)
{
	I2C_sla = sla >> 1;
	I2C_buf = s;
	I2C_num = no;
	I2C_suba = suba;
	
	switch(sub_type)
	{
		// 子地址为“8+x”类型
		case 0:
			I2C_sla = I2C_sla | ((suba >> 8) & 0x07);
			I2C_suba_num = 1;
			break;	
		// 子地址为1个字节 	
		case 1:
			I2C_suba_num = 1;	
			break;
		// 子地址为2个字节
		case 2:
			I2C_suba_num = 2;	
			break;
		// 子地址为3个字节
		case 3:
			I2C_suba_num = 3;	
			break;
		default: break;
	}
	
	if((no == 1) && (I2C_suba_num == 1))
		I2C_state = STATE_WRITE_FINAL;	// 单字节地址或8＋x结构地址发送1个字节数据
	else
		I2C_state = STATE_WRITE_NEXT;	// 多字节地址或发送多个字节数据	
						
	I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, false);		// 主机写操作										
    I2C_suba_num--;
    I2CMasterDataPut(I2C_MASTER_BASE, 
    				(I2C_suba >> (8 * I2C_suba_num)));			// 写子地址高字节
    I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// 突发写操作启动
    
	while(I2C_state != STATE_IDLE);
	if(true == I2CMasterBusy(I2C_MASTER_BASE))
		return (false);
	else
		return (true);
}

/*
*********************************************************************************************************
** 函数名称：ISendStrExt()
** 函数功能：向无子地址器件发送多字节数据。
** 入口参数：sla		器件地址
**           s			要发送的数据
**  		 no			发送的字节个数
** 出口参数：返回值为false时表示操作出错，为true时表示操作正确。
** 说明：	 发送数据字节至少2个字节
*********************************************************************************************************
*/ 
int ISendStrExt(uchar sla, uchar *s, uchar no)
{
	I2C_sla = sla >> 1;
	I2C_buf = s;
	I2C_num = no;
	if (no > 1)								// 发送的字节参数过滤
	{
		if(no == 2)
			I2C_state = STATE_WRITE_FINAL;
		else
			I2C_state = STATE_WRITE_NEXT;
	
		I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, false);	// 主机写操作
    	I2CMasterDataPut(I2C_MASTER_BASE, *I2C_buf++);
    	I2C_num--;
    	I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// 突发写操作启动
        
		while(I2C_state != STATE_IDLE);
		if(true == I2CMasterBusy(I2C_MASTER_BASE))
			return (false);
		else
			return (true);
	}
	else 
		return(false);
}

/*
*********************************************************************************************************
** 函数名称：IRcvByte()
** 函数功能：向无子地址器件读取1字节数据。
** 入口参数：sla		器件地址
**           c			接收数据的变量指针
** 出口参数：返回值为false时表示操作出错，为true时表示操作正确。
** 说明：	使用前要初始化好I2C引脚功能和I2C中断，并已使能I2C主模式
*********************************************************************************************************
*/
int IRcvByte(uchar sla, uchar *c)
{
	I2C_sla = sla >> 1;
	I2C_buf = c;
	I2C_state = STATE_READ_WAIT;
	
	I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, true);	// 主机读操作
	I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);// 启动单次读取
	
	while(I2C_state != STATE_IDLE);
	if(true == I2CMasterBusy(I2C_MASTER_BASE))
		return (false);
	else
		return (true);
}

/*
*********************************************************************************************************
** 函数名称 ：IRcvStr()
** 函数功能 ：向有子地址器件读取N字节数据
** 入口参数 ：	sla			器件从地址
**				suba_type	子地址结构	0－8+X结构  1－单字节地址  2－双字节地址  3－三字节地址
**			  	suba		器件内部物理地址
**			  	*s			将要读取的数据的指针
**			  	no			将要读取的数据的个数
** 出口参数 ：	TRUE		操作成功
**			  	FALSE		操作失败
*********************************************************************************************************
*/
int IRcvStr(uchar sla, ulong suba, uchar sub_type, uchar *s, uchar no)
{
	I2C_sla = sla >> 1;
	I2C_buf = s;
	I2C_num = no;
	I2C_suba = suba;
	I2C_opt = 1;
	
	switch(sub_type)
	{
		// 子地址为“8+x”类型
		case 0:
			I2C_sla = I2C_sla | ((suba >> 8) & 0x07);
			I2C_suba_num = 1;
			break;	
		// 子地址为1个字节 	
		case 1:
			I2C_suba_num = 1;	
			break;
		// 子地址为2个字节
		case 2:
			I2C_suba_num = 2;	
			break;
		// 子地址为3个字节
		case 3:
			I2C_suba_num = 3;	
			break;
		default: break;
	}
	
	if(I2C_suba_num == 1)
	{
		if(I2C_num == 1)
			I2C_state = STATE_READ_ONE;
		else 
		 	I2C_state = STATE_READ_FIRST;
	}
	else
		I2C_state = STATE_WRITE_NEXT;
						
	I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, false);		// 主机写操作										
    I2C_suba_num--;
    I2CMasterDataPut(I2C_MASTER_BASE, 
    				(I2C_suba >> (8 * I2C_suba_num)));			// 写子地址高字节
    I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// 突发写操作启动
    
	while(I2C_state != STATE_IDLE);
	if(true == I2CMasterBusy(I2C_MASTER_BASE))
		return (false);
	else
		return (true);
}

/*
*********************************************************************************************************
** 函数名称：IRvcStrExt()
** 函数功能：向无子地址器件读取N字节数据。
** 入口参数：sla		器件地址
**           s			接收数据的变量指针
**			 no			将要读取的数据的个数
** 出口参数：使用前要初始化好I2C引脚功能和I2C中断，并已使能I2C主模式
** 说明：	至少要读取2个字节
*********************************************************************************************************
*/
int IRvcStrExt(uchar sla, uchar *s, uchar no)
{
	I2C_sla = sla >> 1;
	I2C_buf = s;
	I2C_num = no;
	
	if(I2C_num > 1)
	{
		if(I2C_num == 2)
			I2C_state = STATE_READ_FINAL;
		else
			I2C_state = STATE_READ_NEXT;
			
		I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, true);	// 主机读操作
    	I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);// 突发读操作启动
	
		while(I2C_state != STATE_IDLE);
		if(true == I2CMasterBusy(I2C_MASTER_BASE))
			return (false);
		else
			return (true);
	}
	else
		return (false);
}


/*******************************************************************************************
** 函数名称 ：I2C_ISR()
** 函数功能 ：中断读写数据
** 入口参数 ：无
** 出口参数 ：无
*******************************************************************************************/
void I2C_ISR (void)
{
    I2CMasterIntClear(I2C_MASTER_BASE);		// 清除I2C中断标志
    switch(I2C_state)
    {
        // 空闲状态
        case STATE_IDLE:
        {
            break;
        }
        // 写完单个字节状态
        case STATE_WRITE_ONE:
        {
        	I2C_state = STATE_IDLE;
        	break;
        }
		// 写下一个数据
        case STATE_WRITE_NEXT:
        {
        	// 将下一个字节写入寄存器
            if(I2C_suba_num != 0)
            {
            	I2C_suba_num--;
            	I2CMasterDataPut(I2C_MASTER_BASE, 
            					(I2C_suba >> (8 * I2C_suba_num)));
            	if((I2C_suba_num == 0) && I2C_opt == 1)
            	{
            		if(I2C_num == 1)
            			I2C_state = STATE_READ_ONE;	
            		else
            			I2C_state = STATE_READ_FIRST;	
            	}
            	
            }            
			else
            {
             	I2CMasterDataPut(I2C_MASTER_BASE, *I2C_buf++);
            	I2C_num--;             
            	if(I2C_num == 1)
            	{
                	I2C_state = STATE_WRITE_FINAL;
            	}          
            }
            // 继续执行块写操作(run=1)
            I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
            break;
        }
        // 写最后一个数据
        case STATE_WRITE_FINAL:
        {
            I2CMasterDataPut(I2C_MASTER_BASE, *I2C_buf);
            I2C_num--;
            // 发送停止
            I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
			// 下一个状态为块写完成
            I2C_state= STATE_IDLE;
            break;
        }
 
 		// 读取一个字节
        case STATE_READ_ONE:
        {

            I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, true);

            I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

            I2C_state= STATE_READ_WAIT;

            break;
        }
		// 读取多个字节开始
        case STATE_READ_FIRST:
        {

            I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, true);

            I2CMasterControl(I2C_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_START);

            if(I2C_num == 2)
                I2C_state = STATE_READ_FINAL;
            else
            	I2C_state = STATE_READ_NEXT;

            break;
        }
		// 读取下一个字节
        case STATE_READ_NEXT:
        {

            *I2C_buf++ = I2CMasterDataGet(I2C_MASTER_BASE);
            I2C_num--;

            I2CMasterControl(I2C_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_CONT);

            if(I2C_num == 2)
                I2C_state = STATE_READ_FINAL;

            break;
        }
		// 读取最后一个字节
        case STATE_READ_FINAL:
        {

            *I2C_buf++ = I2CMasterDataGet(I2C_MASTER_BASE);
            I2C_num--;

            I2CMasterControl(I2C_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

            I2C_state= STATE_READ_WAIT;
            break;
        }
		// 等待读取一个字节
        case STATE_READ_WAIT:
        {
 
            *I2C_buf++  = I2CMasterDataGet(I2C_MASTER_BASE); // 读取数据
            I2C_num--;

            I2C_state= STATE_IDLE;
            break;
        }
    }
}
