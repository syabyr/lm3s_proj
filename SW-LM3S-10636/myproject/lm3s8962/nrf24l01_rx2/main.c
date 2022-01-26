#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include "drivers/rit128x96x4.h"
#include "drivers/mydelay.h"
#include "driverlib/timer.h"

#include "nRF24L01.h"
#include <inttypes.h>
#include "uartstdio.h"

#define NUM_SSI_DATA 3

#define GPIO_PA2_SSI0CLK        0x00000801
#define GPIO_PA3_SSI0FSS        0x00000C01
#define GPIO_PA4_SSI0RX         0x00001001
#define GPIO_PA5_SSI0TX         0x00001401


uint16_t counter=0;

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif



int value=0;

const uint64_t addresses[2] = { 0xABABCDCD71LL, 0x544d52687CLL };

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void UART0IntHandler(void)
{
    unsigned long ulStatus;

    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART0_BASE, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE));
    }
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
}

//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
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
    // Initialize the OLED display and write status.
    //
    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //PC5,PC7 EN,CSN
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,1<<5|1<<7);

    //SPI配置
    unsigned long ulDataTx[NUM_SSI_DATA];
    unsigned long ulDataRx[NUM_SSI_DATA];
    unsigned long ulindex;
    unsigned long ultemp=0;


    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                   GPIO_PIN_2);

    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 4000000, 8);
    /*
    GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_DIR_MODE_OUT); 
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_STRENGTH_4MA,
                     GPIO_PIN_TYPE_STD_WPU);  
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_DIR_MODE_OUT);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 , GPIO_STRENGTH_4MA,
                     GPIO_PIN_TYPE_STD_WPU);
    */
    SSIEnable(SSI0_BASE);


    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    //
    // Prompt for text to be entered.
    //
    UARTStdioInit(0);
    UARTSend((unsigned char *)"Enter text:\n\r", 12);
    UARTSend((unsigned char *)"Enter text:\n\r", 12);

    //清零接收缓冲区
    while(SSIDataGetNonBlocking(SSI0_BASE, &ulDataRx[0]))
    {
    }


    //初始化NRF24L01
    set_module_tx();
    nrf_write_reg(CONFIG,0x0a);
    print_byte_register("CONFIG\t",CONFIG,1);

    init_NRF24L01();
    set_module_tx();


    uint8_t data[32];


    openWritingPipe(addresses[0]);
    openReadingPipe(1,addresses[1]);


    //调试关闭
    //nrf_write_reg(EN_RXADDR,0x02);
    nrf_write_reg(RX_PW_P1,0x20);

    //nrf_write_reg(CONFIG,0x0b);
    nrf_write_reg(CONFIG, nrf_read_reg(CONFIG) | _BV(PRIM_RX));
    //nrf_write_reg(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

    if(nrf_read_reg(FEATURE) & _BV(EN_ACK_PAY))
    {
        flush_tx();
    }

    flush_rx();
    //nrf_write_reg(SETUP_AW,0x03);

    printDetails();

    

    Init_Timer_A();

    //将业务数据写入:WR_TX_PLOAD

    uint8_t fifo_status,status,state,i;
    while(1)
    {
        fifo_status=spi_read_reg(FIFO_STATUS);
        if(fifo_status&0x02)
        {
            status=spi_read_reg(STATUS);
            if(status&_BV(RX_DR))
            {
                nrf_rx_packet(data);
                counter++;
            }
            if(status &0x02)
            {
                nrf_write_reg(FLUSH_RX,0xFF);
            }
        }
    }//dead loop
}


void Timer0AIntHandler()
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    UARTprintf("count:%d\r\n",counter);
    counter=0;
}



void Init_Timer_A(void)
{
  // 使能TIMER0模块
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); 
  
  // 禁止定时器A
  // TimerDisable (unsigned long ulBase, unsigned long ulTimer)
  TimerDisable(TIMER0_BASE, TIMER_A);
  
  // 配置TIMER0模块工作在双16位定时器,周期工作模式
  // TimerConfigure (unsignedlongulBase,unsignedlongulConfig)
  TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_PERIODIC);
  
  // 配置8位分频器
  // TimerPrescaleSet (unsigned long ulBase, unsigned long ulTimer, unsigned long ulValue)
  TimerPrescaleSet( TIMER0_BASE, TIMER_A,200);
  
  // 配置定时器周期
  // TimerLoadSet (unsigned long ulBase, unsigned long ulTimer, unsigned long ulValue)
  TimerLoadSet(TIMER0_BASE, TIMER_A, 80000000);
  
  // 使能时钟溢出中断
  // TimerIntEnable (unsigned long ulBase, unsigned long ulIntFlags) 
  TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
  
  // 在NVIC中使能时钟中断
  IntEnable(INT_TIMER0A);
  
  // 使能TIMER A
  // TimerEnable (unsigned long ulBase, unsigned long ulTimer)
  TimerEnable(TIMER0_BASE, TIMER_A);   
}