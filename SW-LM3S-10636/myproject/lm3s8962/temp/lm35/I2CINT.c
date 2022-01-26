/*
---------------------------------------------------------------------------------------------------------
*********************************************************************************************************
** �ļ����� ��	I2CINT.c
** ����˵�� ��	LuminaryӲ��I2C�жϷ�ʽ�������
** ʹ��˵�� ��  ������Ҫ���ú�I2C���߽ӿ�(I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ)
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
** �ж��е�״̬
*******************************************************************************************/ 
#define STATE_IDLE         0					/* ���߿���״̬ 		*/
#define STATE_WRITE_ONE    1					/* д�����ֽ�״̬ 		*/
#define STATE_WRITE_NEXT   2					/* д��һ���ֽ�״̬ 	*/
#define STATE_WRITE_FINAL  3					/* д���һ���ֽ�״̬ 	*/
#define STATE_READ_ONE     4					/* �������ֽ�״̬ 		*/
#define STATE_READ_FIRST   5					/* ����һ���ֽ�״̬ 	*/
#define STATE_READ_NEXT    6					/* ����һ���ֽ�״̬ 	*/
#define STATE_READ_FINAL   7					/* �����һ���ֽ�״̬ 	*/
#define STATE_READ_WAIT    8					/* ���ֽڵȴ�״̬ 		*/

/*******************************************************************************************
** ȫ�ֱ�������
*******************************************************************************************/ 
static volatile uchar 	I2C_sla;				/* I2C�����ӵ�ַ 					*/
static volatile ulong	I2C_suba;				/* I2C�����ڲ��ӵ�ַ 				*/
static volatile uchar 	I2C_suba_num;			/* I2C�ӵ�ַ�ֽ���					*/
static volatile uchar 	*I2C_buf;        		/* ���ݻ�����ָ��  					*/
static volatile ulong   I2C_num;				/* Ҫ��ȡ/д������ݸ��� 			*/
static volatile uchar   I2C_opt; 				/* I2c��д����,0Ϊ������,1Ϊд����	*/
static volatile uchar 	I2C_state = STATE_IDLE; /* �жϷ������ĵ�ǰ״̬  			*/


/*
******************************************************************************************
** ��������: Delays()
** ��������: ��ʱ
** ��ڲ���: dly
** ���ڲ���: ��
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
** �������ƣ�I2CInit()
** �������ܣ�I2C�ӿڳ�ʼ����
** ��ڲ�����spd		�����ٶ�100K(����ֵΪ100000)��400K(����ֵΪ4000000)
**           pri		�ж����ȼ�0��7
** ���ڲ���������ֵΪfalseʱ��ʾ��������Ϊtrueʱ��ʾ������ȷ��
** ˵����
*********************************************************************************************************
*/
int I2CInit(ulong spd, uchar pri)
{
	if ((spd == 400000) || (spd == 100000))
	{
    	// ʹ����ʹ�õ�����
    	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		
    	// ����������ţ��Խ���I2C����
    	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    	 GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_OD);
    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_OD);


    	// ��ʼ��I2C����,��������Ϊ����.
    	if(spd == 400000)
    		I2CMasterInit(I2C_MASTER_BASE, true);
		else
			I2CMasterInit(I2C_MASTER_BASE, false);
		
    	// ʹ�ܴ������ж�
    	IntMasterEnable();
    	
    	// ʹ��I2C�ж�
    	IntEnable(INT_I2C);

    	// ʹ��I2C�����ж�
    	I2CMasterIntEnable(I2C_MASTER_BASE);
    	
    	// ����I2C�жϵ����ȼ�
    	IntPrioritySet(INT_I2C, (pri << 5));
    
    	return(true);
    }
    else
    	return(false);
}

/*
*********************************************************************************************************
** �������ƣ�ISendByte()
** �������ܣ������ӵ�ַ��������1�ֽ����ݡ�
** ��ڲ�����sla		������ַ
**           c			Ҫ���͵�����
** ���ڲ���������ֵΪfalseʱ��ʾ��������Ϊtrueʱ��ʾ������ȷ��
** ˵����	ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
*********************************************************************************************************
*/ 
int ISendByte(uchar sla, uchar c)
{
	I2C_sla = sla >> 1;
	I2C_buf = &c;
	I2C_state = STATE_WRITE_ONE;
	
	I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, false);			// ����д����
	I2CMasterDataPut(I2C_MASTER_BASE, *I2C_buf);					// дһ������
    I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);	// �������η���
	
	while(I2C_state != STATE_IDLE);
	if(true == I2CMasterBusy(I2C_MASTER_BASE))
		return (false);
	else
		return (true);
}

/*
*********************************************************************************************************
** �������� ��ISendStr()
** �������� �������ӵ�ַ���������ַ��ʼд��N�ֽ�����
** ��ڲ��� ��	sla			�����ӵ�ַ
**				suba_type	�ӵ�ַ�ṹ	0��8+X�ṹ  1�����ֽڵ�ַ  2��˫�ֽڵ�ַ  3�����ֽڵ�ַ
**				suba		�����ӵ�ַ
**				s			���ݷ��ͻ�����ָ��
**				no			д��ĸ���
** ���ڲ��� ��	TRUE		�����ɹ�
**				FALSE		����ʧ��
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
		// �ӵ�ַΪ��8+x������
		case 0:
			I2C_sla = I2C_sla | ((suba >> 8) & 0x07);
			I2C_suba_num = 1;
			break;	
		// �ӵ�ַΪ1���ֽ� 	
		case 1:
			I2C_suba_num = 1;	
			break;
		// �ӵ�ַΪ2���ֽ�
		case 2:
			I2C_suba_num = 2;	
			break;
		// �ӵ�ַΪ3���ֽ�
		case 3:
			I2C_suba_num = 3;	
			break;
		default: break;
	}
	
	if((no == 1) && (I2C_suba_num == 1))
		I2C_state = STATE_WRITE_FINAL;	// ���ֽڵ�ַ��8��x�ṹ��ַ����1���ֽ�����
	else
		I2C_state = STATE_WRITE_NEXT;	// ���ֽڵ�ַ���Ͷ���ֽ�����	
						
	I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, false);		// ����д����										
    I2C_suba_num--;
    I2CMasterDataPut(I2C_MASTER_BASE, 
    				(I2C_suba >> (8 * I2C_suba_num)));			// д�ӵ�ַ���ֽ�
    I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// ͻ��д��������
    
	while(I2C_state != STATE_IDLE);
	if(true == I2CMasterBusy(I2C_MASTER_BASE))
		return (false);
	else
		return (true);
}

/*
*********************************************************************************************************
** �������ƣ�ISendStrExt()
** �������ܣ������ӵ�ַ�������Ͷ��ֽ����ݡ�
** ��ڲ�����sla		������ַ
**           s			Ҫ���͵�����
**  		 no			���͵��ֽڸ���
** ���ڲ���������ֵΪfalseʱ��ʾ��������Ϊtrueʱ��ʾ������ȷ��
** ˵����	 ���������ֽ�����2���ֽ�
*********************************************************************************************************
*/ 
int ISendStrExt(uchar sla, uchar *s, uchar no)
{
	I2C_sla = sla >> 1;
	I2C_buf = s;
	I2C_num = no;
	if (no > 1)								// ���͵��ֽڲ�������
	{
		if(no == 2)
			I2C_state = STATE_WRITE_FINAL;
		else
			I2C_state = STATE_WRITE_NEXT;
	
		I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, false);	// ����д����
    	I2CMasterDataPut(I2C_MASTER_BASE, *I2C_buf++);
    	I2C_num--;
    	I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// ͻ��д��������
        
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
** �������ƣ�IRcvByte()
** �������ܣ������ӵ�ַ������ȡ1�ֽ����ݡ�
** ��ڲ�����sla		������ַ
**           c			�������ݵı���ָ��
** ���ڲ���������ֵΪfalseʱ��ʾ��������Ϊtrueʱ��ʾ������ȷ��
** ˵����	ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
*********************************************************************************************************
*/
int IRcvByte(uchar sla, uchar *c)
{
	I2C_sla = sla >> 1;
	I2C_buf = c;
	I2C_state = STATE_READ_WAIT;
	
	I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, true);	// ����������
	I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);// �������ζ�ȡ
	
	while(I2C_state != STATE_IDLE);
	if(true == I2CMasterBusy(I2C_MASTER_BASE))
		return (false);
	else
		return (true);
}

/*
*********************************************************************************************************
** �������� ��IRcvStr()
** �������� �������ӵ�ַ������ȡN�ֽ�����
** ��ڲ��� ��	sla			�����ӵ�ַ
**				suba_type	�ӵ�ַ�ṹ	0��8+X�ṹ  1�����ֽڵ�ַ  2��˫�ֽڵ�ַ  3�����ֽڵ�ַ
**			  	suba		�����ڲ������ַ
**			  	*s			��Ҫ��ȡ�����ݵ�ָ��
**			  	no			��Ҫ��ȡ�����ݵĸ���
** ���ڲ��� ��	TRUE		�����ɹ�
**			  	FALSE		����ʧ��
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
		// �ӵ�ַΪ��8+x������
		case 0:
			I2C_sla = I2C_sla | ((suba >> 8) & 0x07);
			I2C_suba_num = 1;
			break;	
		// �ӵ�ַΪ1���ֽ� 	
		case 1:
			I2C_suba_num = 1;	
			break;
		// �ӵ�ַΪ2���ֽ�
		case 2:
			I2C_suba_num = 2;	
			break;
		// �ӵ�ַΪ3���ֽ�
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
						
	I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, false);		// ����д����										
    I2C_suba_num--;
    I2CMasterDataPut(I2C_MASTER_BASE, 
    				(I2C_suba >> (8 * I2C_suba_num)));			// д�ӵ�ַ���ֽ�
    I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// ͻ��д��������
    
	while(I2C_state != STATE_IDLE);
	if(true == I2CMasterBusy(I2C_MASTER_BASE))
		return (false);
	else
		return (true);
}

/*
*********************************************************************************************************
** �������ƣ�IRvcStrExt()
** �������ܣ������ӵ�ַ������ȡN�ֽ����ݡ�
** ��ڲ�����sla		������ַ
**           s			�������ݵı���ָ��
**			 no			��Ҫ��ȡ�����ݵĸ���
** ���ڲ�����ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
** ˵����	����Ҫ��ȡ2���ֽ�
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
			
		I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, true);	// ����������
    	I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);// ͻ������������
	
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
** �������� ��I2C_ISR()
** �������� ���ж϶�д����
** ��ڲ��� ����
** ���ڲ��� ����
*******************************************************************************************/
void I2C_ISR (void)
{
    I2CMasterIntClear(I2C_MASTER_BASE);		// ���I2C�жϱ�־
    switch(I2C_state)
    {
        // ����״̬
        case STATE_IDLE:
        {
            break;
        }
        // д�굥���ֽ�״̬
        case STATE_WRITE_ONE:
        {
        	I2C_state = STATE_IDLE;
        	break;
        }
		// д��һ������
        case STATE_WRITE_NEXT:
        {
        	// ����һ���ֽ�д��Ĵ���
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
            // ����ִ�п�д����(run=1)
            I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
            break;
        }
        // д���һ������
        case STATE_WRITE_FINAL:
        {
            I2CMasterDataPut(I2C_MASTER_BASE, *I2C_buf);
            I2C_num--;
            // ����ֹͣ
            I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
			// ��һ��״̬Ϊ��д���
            I2C_state= STATE_IDLE;
            break;
        }
 
 		// ��ȡһ���ֽ�
        case STATE_READ_ONE:
        {

            I2CMasterSlaveAddrSet(I2C_MASTER_BASE, I2C_sla, true);

            I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

            I2C_state= STATE_READ_WAIT;

            break;
        }
		// ��ȡ����ֽڿ�ʼ
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
		// ��ȡ��һ���ֽ�
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
		// ��ȡ���һ���ֽ�
        case STATE_READ_FINAL:
        {

            *I2C_buf++ = I2CMasterDataGet(I2C_MASTER_BASE);
            I2C_num--;

            I2CMasterControl(I2C_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

            I2C_state= STATE_READ_WAIT;
            break;
        }
		// �ȴ���ȡһ���ֽ�
        case STATE_READ_WAIT:
        {
 
            *I2C_buf++  = I2CMasterDataGet(I2C_MASTER_BASE); // ��ȡ����
            I2C_num--;

            I2C_state= STATE_IDLE;
            break;
        }
    }
}
