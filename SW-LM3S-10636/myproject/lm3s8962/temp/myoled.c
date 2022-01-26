
#ifndef DEBUG
#define DEBUG 1
#endif
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "oled.h"
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Hello World (hello)</h1>
//!
//! A very simple ``hello world'' example.  It simply displays ``hello world''
//! on the OLED and is a starting point for more complicated applications.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


//*****************************************************************************
//
// Print "Hello world!" to the OLED on the Stellaris evaluation board.
//
//*****************************************************************************
int
main(void)
{

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);


    //
    // Initialize the OLED display.
    //
    OLED_Init(1000000);

    //
    // Hello!
 //  OLED_StringDraw("Hello World!", 30, 24, 15);

static const unsigned char sin[]={
47,43,38,34,29,25,21,17,
14,10,8,5,3,2,0,0,
0,0,0,2,3,5,8,10,
14,17,21,25,29,34,38,43,
47,52,57,61,66,70,74,78,
81,85,87,90,92,93,95,95,
95,95,95,93,92,90,87,85,
81,78,74,70,66,61,57,52,

48,43,38,34,29,25,21,17,
14,10,8,5,3,2,0,0,
0,0,0,2,3,5,8,10,
14,17,21,25,29,34,38,43,
47,52,57,61,66,70,74,78,
81,85,87,90,92,93,95,95,
95,95,95,93,92,90,87,85,
81,78,74,70,66,61,57,52
};


//char buf[]={0xff,0xf0,0x0f,0x00};
static const unsigned char a[]={0xff};
static const unsigned char b[]={0xf0};
static const unsigned char c[]={0x0f};


//OLED_ImageDraw(a,0,sin[0],2,1);

//OLED_ImageDraw(b,2,sin[1],2,1);
//OLED_ImageDraw(c,2,sin[2],2,1);


/*
for(int i=0;i<128;i++)
{
	buff[i][sin[i]]&=0xf0;
	buff[i][sin[i+1]]&=0x0f;
}
*/
unsigned char *line;
line=(unsigned char *)&sin[128];
//OLED_LineDraw(line);
//OLED_Rewrite(line);		//works fine.
/*
while(i<128)
{
//	if(sin[i]==sin[i+1])
//		OLED_ImageDraw(a,i,sin[i],2,1);
//	else
	{
		OLED_ImageDraw(b,i,sin[i],2,1);
		OLED_ImageDraw(c,i+1,sin[i+1],2,1);
	}
	i=i+2;
}
*/
    //
    // Finished.
    //
    while(1)
    {
    }
}
