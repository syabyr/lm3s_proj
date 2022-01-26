#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/flash.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/debug.h"

#include "grlib/grlib.h"
#include "utils/locator.h"
#include "utils/swupdate.h"
#include "utils/ustdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include <stdlib.h>
#include "320x480_ILI9481.h"
#include "mydelay.h"
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"
#include "utils/ustdlib.h"
#include "ds18b20.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//温度部分变量定义
uint16_t temperture_raw;
//整数部分和小数部分
uint8_t int_part,frac_part;
//标志位,为1的时候表示总线上没有传感器
uint8_t is_sensor_present=0;
//标志位,为1的时候说明温度为零下
uint8_t is_subzero=0;

char show[15];

uint8_t sp[DS18X20_SP_SIZE];
//for sdcard
static FATFS g_sFatFs;
static FIL g_sFileObject;
WORD *bw="Hello world";
char *pshow=&show;

//sensors
#define MAXSENSORS 1
uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];
uint8_t buffer[sizeof(int)*8+1];
uint8_t search_sensors(void)
{
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff,nSensors=0;
//	UARTprintf("Scanning Bus for DS18X20.\n");
	for(diff=OW_SEARCH_FIRST;diff != OW_LAST_DEVICE && nSensors<MAXSENSORS;)
	{
		DS18X20_find_sensor(&diff,&id[0]);
		if(diff==OW_PRESENCE_ERR)
		{
//			UARTprintf("No Sensors found.\n");
			break;
		}
		if(diff==OW_DATA_ERR)
		{
//			UARTprintf("Bus Error.\n");
			break;
		}
		for(i=0;i<OW_ROMCODE_SIZE;i++)
			gSensorIDs[nSensors][i]=id[i];
		nSensors++;
	}
	return nSensors;
}




int main(void)
{
	int i;
	tContext sContext;
	tRectangle sRect;
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	//使能温度传感器所使用的端口

	LCD_320x480_ILI9481Init();
	LCD_320x480_ILI9481BacklightOn();		//打开背光


	GrContextInit(&sContext,&g_sLCD_320x480_ILI9481);
	sRect.sXMin=0;
	sRect.sYMin=0;
	sRect.sXMax=GrContextDpyWidthGet(&sContext) - 1;
	sRect.sYMax=40;
	GrContextForegroundSet(&sContext, ClrBlue);
	GrRectFill(&sContext, &sRect);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &sRect);

	#define DATA_BUFFER_SIZE 10
	char pucBuf[DATA_BUFFER_SIZE];
	char *pt=&pucBuf;

	pucBuf[0]=' ';
	pucBuf[1]='a';
	pucBuf[2]='b';
	pucBuf[3]='c';
	pucBuf[4]='d';
	pucBuf[5]='e';
	pucBuf[6]='f';
	pucBuf[10]='o';

//	GrContextFontSet(&sContext, &g_sFontCm30b);
//	GrStringDrawCentered(&sContext,pt, -1,
//		60,96, 1);

	if(is_sensor_present=search_sensors())	//表明有传感器在总线上


	{
		GrContextForegroundSet(&sContext, ClrGreen);
		GrCircleFill(&sContext, 20,20,15);
	}
	else
	{
		GrContextForegroundSet(&sContext, ClrRed);
		GrCircleFill(&sContext, 20,20,15);
		GrContextFontSet(&sContext, &g_sFontCm30b);
		GrStringDrawCentered(&sContext, "Check Sensors then restart", -5,
		GrContextDpyWidthGet(&sContext) / 2, 20, 0);
		while(1);
	}

	while(1)
	{
		if(ow_reset())
		{
		GrContextForegroundSet(&sContext, ClrRed);
		GrCircleFill(&sContext, 20,20,15);
		}
		else
		{
			GrContextForegroundSet(&sContext, ClrGreen);
			GrCircleFill(&sContext, 20,20,15);


			if(ow_get_in())
			{

				GrContextFontSet(&sContext, &g_sFontCm30b);
				GrStringDrawCentered(&sContext,pt, -1,60,96, 1);
			}

		}
/*
		{



			if(ow_get_in())
			{
				ow_command(DS18X20_CONVERT_T,NULL);
				_delay_ms(DS18B20_TCONV_12BIT);
				ow_reset();
				ow_command(DS18X20_READ,gSensorIDs[0][0]);
				for(i=0;i<DS18X20_SP_SIZE;i++)
					sp[i]=ow_byte_rd();
					if(crc8(&sp[0],DS18X20_SP_SIZE))
						return DS18X20_ERROR_CRC;
			//		DS18X20_meas_to_cel(id[0],sp,subzero,cel,cel_frac_bits);
					temperture_raw=sp[0];
					temperture_raw|=((uint16_t)sp[1])<<8;
					if(temperture_raw&0x8000)
					{
						is_subzero=1;
						temperture_raw^=0xffff;
						temperture_raw++;
					}
					else is_subzero=0;
					i=sp[DS18B20_CONF_REG];
					if((i&DS18B20_12_BIT)==DS18B20_12_BIT)
						;
					else if((i&DS18B20_11_BIT)==DS18B20_11_BIT)
						temperture_raw&=~(DS18B20_11_BIT_UNDF);
					else if((i&DS18B20_10_BIT)==DS18B20_10_BIT)
						temperture_raw&=~(DS18B20_10_BIT_UNDF);
					else
						temperture_raw&=~(DS18B20_9_BIT_UNDF);
					int_part=(uint8_t)(temperture_raw>>4);
					frac_part=(uint8_t)(temperture_raw&0x000F);

					i=0;
					if(is_subzero)
					{
						show[i]='-';
						i++;
					}
					if(int_part>99)
					{
						show[i]='1';
						i++;
						int_part-=100;
						int temp;
						temp=int_part/10;
						show[i]='0'+temp;
						i++;
						temp=int_part%10;
						show[i]='0'+temp;
						i++;
						
					}
					else if(int_part>9)
					{
						int temp;
						temp=int_part/10;
						show[i]='0'+temp;
						i++;
						temp=int_part%10;
						show[i]='0'+temp;
						i++;
					}
					else
					{
						show[i]='0'+int_part;
						i++;
					}
					show[i]='.';
					i++;
					show[i]='\n';
					i++;
					show[0]=' ';
					show[1]='a';
					show[2]='b';
					show[3]='c';
					show[4]=' ';
				//	GrStringDrawCentered(&sContext,show, i,
				//	60,96, 1);

				return DS18X20_OK;
			}
			else
				return DS18X20_START_FAIL;
		}
	*/
		_delay_ms(1000);
	}




//	usprintf(c,"%d",a);
    GrContextFontSet(&sContext, &g_sFontCm48b);
    GrStringDrawCentered(&sContext,"hello", -1,
                         GrContextDpyWidthGet(&sContext) / 2,
                         ((GrContextDpyHeightGet(&sContext) - 24) / 2) + 24,
                         ClrDarkRed);

	_delay_ms(3000);
//	usprintf(d,"%d",a);


    GrContextFontSet(&sContext, &g_sFontCm48b);
    GrStringDrawCentered(&sContext,"a", -1,
                         GrContextDpyWidthGet(&sContext) / 2,
                         ((GrContextDpyHeightGet(&sContext) - 24) / 2) + 24,
                         ClrDarkRed);

/*
	tRectangle myRect;
//	tContext myContext;
//	GrContextInit(&sContext,&g_sLCD_320x480_ILI9481);
	myRect.sXMin=0;
	myRect.sYMin=0;
	myRect.sXMax=GrContextDpyWidthGet(&sContext)-1;
	myRect.sYMax=320 - 1;
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectFill(&sContext, &myRect);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &myRect);
	GrContextForegroundSet(&sContext, ClrRed);
*/
/*
	tContext myContext;
	GrContextInit(&myContext,&g_sLCD_320x480_ILI9481);
	
	GrContextForegroundSet(&myContext, ClrGreen);
	GrCircleFill(&myContext, 160,160,80);
	GrContextForegroundSet(&sContext, ClrRed);
	GrCircleFill(&sContext, 280,160,81);
*/
	/*
	unsigned long i,j;
	GrLineDraw(&sContext,1,20,470,310);
	for(j=0;j<300;j++)
	for(i=0;i<300;i++)
	{
		LCD_320x480_ILI9481PixelDraw(j,i,0xff);
		_delay_us(10);
	}
	*/
	GrLineDraw(&sContext,100,100,100,300);
	GrLineDraw(&sContext,100,100,300,100);
	GrLineDraw(&sContext,100,100,200,300);

	//绘制一个点
//	for(i=0;i<100;i++)
//	LCD_240x480_HX8352PixelDraw(i,i,0xffff);
	//绘制一条水平的线段
//	LCD_240x480_HX8352LineDrawH(NULL,0,100,0,0x00ff);
	//绘制一条垂直的线段
//	LCD_240x480_HX8352LineDrawV(NULL,0,0,100,0xff00);

	while(1)
	;

	while(1)
	{
		;
	}

	return 0;
}
