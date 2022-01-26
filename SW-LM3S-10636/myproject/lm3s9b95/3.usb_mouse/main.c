#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/usb.h"
#include "driverlib/timer.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidmouse.h"
#include "uartstdio.h"
#include "usb_mouse_structs.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>USB HID Mouse Device (usb_dev_mouse)</h1>
//!
//! This example application turns the evaluation board into a USB mouse
//! supporting the Human Interface Device class.  Presses on the navigation
//! control on the evaluation board are translated into mouse movement and
//! button press messages in HID reports sent to the USB host allowing the
//! evaluation board to control the mouse pointer on the host system.
//
//*****************************************************************************

//*****************************************************************************
//
// Debug-related definitions and declarations.
//
// Debug output is available via UART0 if DEBUG is defined during build.
//
//*****************************************************************************
#ifdef DEBUG
//*****************************************************************************
//
// Map all debug print calls to UARTprintf in debug builds.
//
//*****************************************************************************
#define DEBUG_PRINT UARTprintf

#else

//*****************************************************************************
//
// Compile out all debug print calls in release builds.
//
//*****************************************************************************
#define DEBUG_PRINT while(0) ((int (*)(char *, ...))0)
#endif

//*****************************************************************************
//
// The parameters to control the USB mux on the LM3S3748 board.
//
//*****************************************************************************
#define USB_MUX_GPIO_PERIPH     SYSCTL_PERIPH_GPIOH
#define USB_MUX_GPIO_BASE       GPIO_PORTH_BASE
#define USB_MUX_GPIO_PIN        GPIO_PIN_2
#define USB_MUX_SEL_DEVICE      USB_MUX_GPIO_PIN
#define USB_MUX_SEL_HOST        0

//*****************************************************************************
//
// The system tick timer rate.
//
//*****************************************************************************
#define SYSTICKS_PER_SECOND 100
#define MS_PER_SYSTICK (1000 / SYSTICKS_PER_SECOND)

//*****************************************************************************
//
// Holds command bits used to signal the main loop to perform various tasks.
//
//*****************************************************************************

//*****************************************************************************
//
// A flag used to indicate whether or not we are currently connected to the USB
// host.
//
//*****************************************************************************
volatile tBoolean g_bConnected;

//*****************************************************************************
//
// Global system tick counter holds elapsed time since the application started
// expressed in 100ths of a second.
//
//*****************************************************************************
volatile unsigned long g_ulSysTickCount;

//*****************************************************************************
//
// The number of system ticks to wait for each USB packet to be sent before
// we assume the host has disconnected.  The value 50 equates to half a second.
//
//*****************************************************************************
#define MAX_SEND_DELAY          50

//*****************************************************************************
//
// This enumeration holds the various states that the mouse can be in during
// normal operation.
//
//*****************************************************************************
volatile enum
{
    //
    // Unconfigured.
    //
    MOUSE_STATE_UNCONFIGURED,

    //
    // No keys to send and not waiting on data.
    //
    MOUSE_STATE_IDLE,

    //
    // Waiting on data to be sent out.
    //
    MOUSE_STATE_SENDING
}
g_eMouseState = MOUSE_STATE_UNCONFIGURED;


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


unsigned long
MouseHandler(void *pvCBData, unsigned long ulEvent,
             unsigned long ulMsgData, void *pvMsgData)
{
    switch(ulEvent)
    {
        //
        // The USB host has connected to and configured the device.
        //
        case USB_EVENT_CONNECTED:
        {
            DEBUG_PRINT("Host connected.\n");
            g_eMouseState = MOUSE_STATE_IDLE;
            g_bConnected = true;
            break;
        }

        //
        // The USB host has disconnected from the device.
        //
        case USB_EVENT_DISCONNECTED:
        {
            DEBUG_PRINT("Host disconnected.\n");
            g_bConnected = false;
            g_eMouseState = MOUSE_STATE_UNCONFIGURED;
            break;
        }

        //
        // A report was sent to the host. We are not free to send another.
        //
        case USB_EVENT_TX_COMPLETE:
        {
            DEBUG_PRINT("TX complete.\n");
            g_eMouseState = MOUSE_STATE_IDLE;
            break;
        }

    }
    return(0);
}


//***************************************************************************
//
// Wait for a period of time for the state to become idle.
//
// \param ulTimeoutTick is the number of system ticks to wait before
// declaring a timeout and returning \b false.
//
// This function polls the current keyboard state for ulTimeoutTicks system
// ticks waiting for it to become idle.  If the state becomes idle, the
// function returns true.  If it ulTimeoutTicks occur prior to the state
// becoming idle, false is returned to indicate a timeout.
//
// \return Returns \b true on success or \b false on timeout.
//
//***************************************************************************
tBoolean WaitForSendIdle(unsigned long ulTimeoutTicks)
{
    unsigned long ulStart;
    unsigned long ulNow;
    unsigned long ulElapsed;

    ulStart = g_ulSysTickCount;
    ulElapsed = 0;

    while (ulElapsed < ulTimeoutTicks)
    {
        //
        // Is the keyboard is idle, return immediately.
        //
        if (g_eMouseState == MOUSE_STATE_IDLE)
        {
            return (true);
        }

        //
        // Determine how much time has elapsed since we started waiting.  This
        // should be safe across a wrap of g_ulSysTickCount.  I suspect you
        // won't likely leave the app running for the 497.1 days it will take
        // for this to occur but you never know...
        //
        ulNow = g_ulSysTickCount;
        ulElapsed = (ulStart < ulNow) ? (ulNow - ulStart)
                        : (((unsigned long)0xFFFFFFFF - ulStart) + ulNow + 1);
    }

    //
    // If we get here, we timed out so return a bad return code to let the
    // caller know.
    //
    return (false);
}


//*****************************************************************************
//
// This is the interrupt handler for the SysTick interrupt.  It is called
// periodically and updates a global tick counter then sets a flag to tell the
// main loop to check the button state.
//
//*****************************************************************************
void
SysTickHandler(void)
{
    g_ulSysTickCount++;
}

//*****************************************************************************
//
// This is the main loop that runs the application.
//
//*****************************************************************************
int
main(void)
{
//    tRectangle sRect;

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    //
    // Enable the USB mux GPIO.
    //
    SysCtlPeripheralEnable(USB_MUX_GPIO_PERIPH);

    //
    // The LM3S3748 board uses a USB mux that must be switched to use the
    // host connector and not the device connecter.
    //
    GPIOPinTypeGPIOOutput(USB_MUX_GPIO_BASE, USB_MUX_GPIO_PIN);
    GPIOPinWrite(USB_MUX_GPIO_BASE, USB_MUX_GPIO_PIN, USB_MUX_SEL_DEVICE);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIODirModeSet(GPIO_PORTC_BASE,GPIO_PIN_4|GPIO_PIN_5,GPIO_DIR_MODE_IN);
	GPIODirModeSet(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5,GPIO_DIR_MODE_IN);
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_5,GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_4|GPIO_PIN_5,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_5,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
//#ifdef DEBUG
    //
    // Configure the relevant pins such that UART0 owns them.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //
    // Open UART0 for debug output.
    //
    UARTStdioInit(0);
//#endif


    // Set the system tick to fire 100 times per second.
    //
    SysTickPeriodSet(SysCtlClockGet() / SYSTICKS_PER_SECOND);
    SysTickIntEnable();
    SysTickEnable();

    
    USBDHIDMouseInit(0, (tUSBDHIDMouseDevice *)&g_sMouseDevice);




    //
    // Drop into the main loop.
    //
    while(1)
    {

        // Wait for USB configuration to complete.
        //
        while(!g_bConnected)
        {
        }


        // Now keep processing the mouse as long as the host is connected.
        //
        while(g_bConnected)
        {
		    unsigned long ulRetcode;
		    char cDeltaX, cDeltaY;
			char cbtndown;
		    tBoolean bSuccess;
			cDeltaX = 0;
	        cDeltaY = 0;
			cbtndown = 0;
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_5) == 0)
			{
				cDeltaX-=3;
			}
			if(GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_5) == 0)
			{
				cDeltaY-=3;
			}
			if(GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_4) == 0)
			{
				cDeltaY+=3;
			}
			if(GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_5) == 0)
			{
				cDeltaX+=3;
			}
			if(GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4) == 0)
			{
				cbtndown |= 1;
			}
//			if(GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_3) == 0)
//			{
//				cbtndown |= 2;
//			}
	
	        g_eMouseState = MOUSE_STATE_SENDING;
	        ulRetcode = USBDHIDMouseStateChange((void *)&g_sMouseDevice, cDeltaX,
	                                            cDeltaY,
	                                            cbtndown);
	
	        //
	        // Did we schedule the report for transmission?
	        //
	        if(ulRetcode == MOUSE_SUCCESS)
	        {
	            //
	            // Wait for the host to acknowledge the transmission if all went
	            // well.
	            //
	            bSuccess = WaitForSendIdle(MAX_SEND_DELAY);
	
	            //
	            // Did we time out waiting for the packet to be sent?
	            //
	            if (!bSuccess)
	            {
	                //
	                // Yes - assume the host disconnected and go back to
	                // waiting for a new connection.
	                //
	                DEBUG_PRINT("Send timed out!\n");
	                g_bConnected = 0;
	            }
	        }
	        else
	        {
	            //
	            // An error was reported when trying to send the report. This may
	            // be due to host disconnection but could also be due to a clash
	            // between our attempt to send a report and the driver sending the
	            // last report in response to an idle timer timeout so we don't
	            // jump to the conclusion that we were disconnected in this case.
	            //
	            DEBUG_PRINT("Can't send report.\n");
	            bSuccess = false;
	        }	
        }
    }
}
