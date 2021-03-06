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
#include "drivers/ds18b20.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

#define MAXSENSORS 1
uint8_t gSensorIDs[OW_ROMCODE_SIZE];
uint8_t subzero,cel;
uint16_t cel_frac_bits;

char show[15];
char *pshow=&show;

//for sdcard
static FATFS g_sFatFs;
static FIL g_sFileObject;
	const uint8_t buff[480]=
{
127,134,142,150,158,166,173,181,
188,195,201,207,213,219,224,229,
234,238,241,245,247,250,251,252,
253,254,253,252,251,250,247,245,
241,238,234,229,224,219,213,207,
201,195,188,181,173,166,158,150,
142,134,127,119,111,103,95,87,
80,72,65,58,52,46,40,34,

29,24,19,15,12,8,6,3,
2,1,0,0,0,1,2,3,
6,8,12,15,19,24,29,34,
40,46,52,58,65,72,80,87,
95,103,111,119,126,134,142,150,
158,166,173,181,188,195,201,207,
213,219,224,229,234,238,241,245,
247,250,251,252,253,254,253,252,

251,250,247,245,241,238,234,229,
224,219,213,207,201,195,188,181,
173,166,158,150,142,134,127,119,
111,103,95,87,80,72,65,58,
52,46,40,34,29,24,19,15,
12,8,6,3,2,1,0,0,
0,1,2,3,6,8,12,15,
19,24,29,34,40,46,52,58,

65,72,80,87,95,103,111,119,
126,134,142,150,158,166,173,181,
188,195,201,207,213,219,224,229,
234,238,241,245,247,250,251,252,
253,254,253,252,251,250,247,245,
241,238,234,229,224,219,213,207,
201,195,188,181,173,166,158,150,
142,134,127,119,111,103,95,87,

80,72,65,58,52,46,40,34,
29,24,19,15,12,8,6,3,
2,1,0,0,0,1,2,3,
6,8,12,15,19,24,29,34,
40,46,52,58,65,72,80,87,
95,103,111,119,126,134,142,150,
158,166,173,181,188,195,201,207,
213,219,224,229,234,238,241,245,

247,250,251,252,253,254,253,252,
251,250,247,245,241,238,234,229,
224,219,213,207,201,195,188,181,
173,166,158,150,142,134,127,119,
111,103,95,87,80,72,65,58,
52,46,40,34,29,24,19,15,
12,8,6,3,2,1,0,0,
0,1,2,3,6,8,12,15,

19,24,29,34,40,46,52,58,
65,72,80,87,95,103,111,119

};




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
			gSensorIDs[i]=id[i];
		nSensors++;
	}
	return nSensors;
}


void draw_neb(uint8_t buf[],uint16_t i)
{
	uint16_t middle;
	if(buf[i-1]==buf[i])
	{
		LCD_320x480_ILI9481LineDrawH(NULL,i-1,i,319-buf[i],ClrBlack);
	}
	else if(buf[i-1]<buf[i])
	{
		middle=(buf[i]+buf[i-1])/2;
		LCD_320x480_ILI9481LineDrawV(NULL,i-1,319-middle,319-buf[i-1],ClrBlack);
		LCD_320x480_ILI9481LineDrawV(NULL,i,319-buf[i],319-middle,ClrBlack);
	}
	else if(buf[i-1]>buf[i])
	{
		middle=(buf[i-1]+buf[i])/2;
		LCD_320x480_ILI9481LineDrawV(NULL,i-1,319-buf[i-1],319-middle,ClrBlack);
		LCD_320x480_ILI9481LineDrawV(NULL,i,319-middle,319-buf[i],ClrBlack);	
	}
	
}


void clean_neb(uint8_t buf[],uint16_t i)
{
	uint16_t middle;
	if(buf[i-1]==buf[i])
	{
		LCD_320x480_ILI9481LineDrawH(NULL,i-1,i,319-buf[i],ClrWhite);
	}
	else if(buf[i-1]<buf[i])
	{
		middle=(buf[i]+buf[i-1])/2;
		LCD_320x480_ILI9481LineDrawV(NULL,i-1,319-middle,319-buf[i-1],ClrWhite);
		LCD_320x480_ILI9481LineDrawV(NULL,i,319-buf[i],319-middle,ClrWhite);
	}
	else if(buf[i-1]>buf[i])
	{
		middle=(buf[i-1]+buf[i])/2;
		LCD_320x480_ILI9481LineDrawV(NULL,i-1,319-buf[i-1],319-middle,ClrWhite);
		LCD_320x480_ILI9481LineDrawV(NULL,i,319-middle,319-buf[i],ClrWhite);	
	}
	
}

int main(void)
{
	uint8_t i,j,temp;
	uint16_t k;
	uint8_t nSensors;

	tContext sContext;
	tRectangle sRect;
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
	//???????????????????????????????????????

	LCD_320x480_ILI9481Init();
	LCD_320x480_ILI9481BacklightOn();		//????????????


	GrContextInit(&sContext,&g_sLCD_320x480_ILI9481);
	sRect.sXMin=0;
	sRect.sYMin=0;
	sRect.sXMax=GrContextDpyWidthGet(&sContext) - 1;
	sRect.sYMax=40;
	GrContextForegroundSet(&sContext, ClrBlue);
	GrRectFill(&sContext, &sRect);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &sRect);

	//????????????
	tRectangle elecRect;
	elecRect.sXMin=0;
	elecRect.sYMin=320-256;
	elecRect.sXMax=480-1;
	elecRect.sYMax=320-1;
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectFill(&sContext, &elecRect);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &elecRect);
/*
	while(1)
	{
		for(k=1;k<400;k++)
		{
			draw_neb(&buff[0],k);
			_delay_ms(10);
		}
		for(k=1;k<400;k++)
		{
			clean_neb(&buff[0],k);
			_delay_ms(10);
		}
	//LCD_320x480_ILI9481PixelDraw(k,319-buf[k],0xff);
	}
*/


//	GrContextFontSet(&sContext, &g_sFontCm30b);
//	GrStringDrawCentered(&sContext,pt, -1,
//		60,96, 1);

	if(nSensors=search_sensors())	//??????????????????????????????


	{
		GrContextForegroundSet(&sContext, ClrLawnGreen);
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
	

		for(k=1;k<400;k++)
		{
			draw_neb(&buff[0],k);
			_delay_ms(10);
		}

		if(DS18X20_get_temp(&gSensorIDs[0],&subzero,&cel,&cel_frac_bits))
		{
			GrContextForegroundSet(&sContext, ClrRed);
			GrCircleFill(&sContext, 20,20,15);
		
			GrContextFontSet(&sContext, &g_sFontCm30b);
			GrContextBackgroundSet(&sContext, ClrBlue);
			GrStringDraw(&sContext,"bus error",11,40,10, 10);
			_delay_ms(1000);
		}
		else
		{
			GrContextForegroundSet(&sContext, ClrBlue);
			GrContextBackgroundSet(&sContext, ClrBlue);
			GrCircleFill(&sContext, 20,20,15);
//			_delay_ms(1000);
			if(subzero)
				show[0]='-';
			else
				show[0]=' ';
			if(cel>99)
			{
				show[1]='1';
				cel-=100;
			}
			else
				show[1]='0';
			temp=cel/10;
				show[2]='0'+temp;
			temp=cel%10;
				show[3]='0'+temp;
			show[4]='.';
			cel_frac_bits*=625;
			temp=cel_frac_bits/1000;
			show[5]='0'+temp;
			cel_frac_bits-=temp*1000;
			temp=cel_frac_bits/100;
			show[6]='0'+temp;
			cel_frac_bits-=temp*100;
			temp=cel_frac_bits/10;
			show[7]='0'+temp;
			show[8]=(cel_frac_bits%10)+'0';
			show[9]='\'';
			show[10]='C';
			GrContextForegroundSet(&sContext, ClrLawnGreen);
			GrContextBackgroundSet(&sContext, ClrBlue);
			GrCircleFill(&sContext, 20,20,15);
			GrContextFontSet(&sContext, &g_sFontCm30b);
			GrStringDraw(&sContext,pshow, 11,40,10, 10);
		}



		for(k=1;k<400;k++)
		{
			clean_neb(&buff[0],k);
			_delay_ms(10);
		}


		if(DS18X20_get_temp(&gSensorIDs[0],&subzero,&cel,&cel_frac_bits))
		{
			GrContextForegroundSet(&sContext, ClrRed);
			GrCircleFill(&sContext, 20,20,15);
		
			GrContextFontSet(&sContext, &g_sFontCm30b);
			GrContextBackgroundSet(&sContext, ClrBlue);
			GrStringDraw(&sContext,"bus error",11,40,10, 10);
			_delay_ms(1000);
		}
		else
		{
			GrContextForegroundSet(&sContext, ClrBlue);
			GrContextBackgroundSet(&sContext, ClrBlue);
			GrCircleFill(&sContext, 20,20,15);
//			_delay_ms(1000);
			if(subzero)
				show[0]='-';
			else
				show[0]=' ';
			if(cel>99)
			{
				show[1]='1';
				cel-=100;
			}
			else
				show[1]='0';
			temp=cel/10;
				show[2]='0'+temp;
			temp=cel%10;
				show[3]='0'+temp;
			show[4]='.';
			cel_frac_bits*=625;
			temp=cel_frac_bits/1000;
			show[5]='0'+temp;
			cel_frac_bits-=temp*1000;
			temp=cel_frac_bits/100;
			show[6]='0'+temp;
			cel_frac_bits-=temp*100;
			temp=cel_frac_bits/10;
			show[7]='0'+temp;
			show[8]=(cel_frac_bits%10)+'0';
			show[9]='\'';
			show[10]='C';
			GrContextForegroundSet(&sContext, ClrLawnGreen);
			GrContextBackgroundSet(&sContext, ClrBlue);
			GrCircleFill(&sContext, 20,20,15);
			GrContextFontSet(&sContext, &g_sFontCm30b);
			GrStringDraw(&sContext,pshow, 11,40,10, 10);
		}
	}




	while(1)
	{
		if(ow_reset())
		{
		GrContextForegroundSet(&sContext, ClrRed);
		GrCircleFill(&sContext, 20,20,15);
		
		GrContextFontSet(&sContext, &g_sFontCm30b);
		GrStringDrawCentered(&sContext,"bus error", -1,60,96, 1);
		}
		else
		{
			GrContextForegroundSet(&sContext, ClrGreen);
			GrCircleFill(&sContext, 20,20,15);

		if(DS18X20_start_meas()==DS18X20_OK)
		{
			_delay_ms(DS18B20_TCONV_12BIT);
	//		for(i=0;i<nSensors;i++)
	//		{
	//			UARTprintf("Sensor# %d =",i);
				if(DS18X20_read_meas(&gSensorIDs[0],&subzero,&cel,&cel_frac_bits)==DS18X20_OK)
				{
					cel=103;
					if(subzero)
						show[0]='-';
					else
						show[0]='+';
					if(cel>99)
					{
						show[1]='1';
						cel-=100;
					}
					else
						show[1]='0';

					temp=cel/10;
					show[2]='0'+temp;
					cel-=10*temp;
					show[3]='0'+cel;
					show[4]='.';
	//				UARTprintf("%d.",(int8_t)cel);
//					itoa(cel_frac_bits*DS18X20_FRACCONV,(char)buffer,10);
//					for(i=0;i<4-strlen(buffer);i++)
//					UARTprintf("0");
//					UARTprintf("%d C`\n",cel_frac_bits*DS18X20_FRACCONV);


					GrContextFontSet(&sContext, &g_sFontCm30b);
					GrStringDrawCentered(&sContext,pshow, -1,60,96, 1);
				}
//				else
//					UARTprintf("CRC Error.\n");
	//		}
		}




		}

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

	//???????????????
//	for(i=0;i<100;i++)
//	LCD_240x480_HX8352PixelDraw(i,i,0xffff);
	//???????????????????????????
//	LCD_240x480_HX8352LineDrawH(NULL,0,100,0,0x00ff);
	//???????????????????????????
//	LCD_240x480_HX8352LineDrawV(NULL,0,0,100,0xff00);

	while(1)
	;

	while(1)
	{
		;
	}

	return 0;
}
