/*
Startup file for LM3S8962.
Modified in Mar.28.2010
Template file,need modify for interupt function.
*/

//forward declaration of the default fault handlers
void ResetHandler(void);
static void NmiHandler(void);
static void FaultHandler(void);
static void DefaultHandler(void);


//entry point for app
extern int main(void);


//constructs from linker
extern unsigned long _text;
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
extern unsigned long _heap;
extern unsigned long _eheap;
extern unsigned long _stack;
extern unsigned long _estack;


//vectors table
__attribute__ ((section(".vectors")))
void (* const _vectorstable[])(void) =
{
	&_estack,        // initial stack pointer
	ResetHandler,    // Reset handler
	NmiHandler,      // NMI handler
	FaultHandler,    // hard fault handler
	DefaultHandler,  // MPU fault handler
	DefaultHandler,  // bus fault handler
	DefaultHandler,  // usage fault handler
	0,               // reserved
	0,               // reserved
	0,               // reserved
	0,               // reserved
	DefaultHandler,  // SVCall handler
	DefaultHandler,  // Debug monitor handler
	0,               // reserved
	DefaultHandler,  // PendSV handler
	DefaultHandler,  // SysTick handler
	DefaultHandler,  // GPIO Port A
	DefaultHandler,  // GPIO Port B
	DefaultHandler,  // GPIO Port C
	DefaultHandler,  // GPIO Port D
	DefaultHandler,  // GPIO Port E
	DefaultHandler,  // UART0 Rx and Tx
	DefaultHandler,  // UART1 Rx and Tx
	DefaultHandler,  // SSI0 Rx and Tx
	DefaultHandler,  // I2C0 Master and Slave
	DefaultHandler,  // PWM Fault
	DefaultHandler,  // PWM Generator 0
	DefaultHandler,  // PWM Generator 1
	DefaultHandler,  // PWM Generator 2
	DefaultHandler,  // Quadrature Encoder 0
	DefaultHandler,  // ADC Sequence 0
	DefaultHandler,  // ADC Sequence 1
	DefaultHandler,  // ADC Sequence 2
	DefaultHandler,  // ADC Sequence 3
	DefaultHandler,  // Watchdog timer
	DefaultHandler,  // Timer 0 subtimer A
	DefaultHandler,  // Timer 0 subtimer B
	DefaultHandler,  // Timer 1 subtimer A
	DefaultHandler,  // Timer 1 subtimer B
	DefaultHandler,  // Timer 2 subtimer A
	DefaultHandler,  // Timer 2 subtimer B
	DefaultHandler,  // Analog Comparator 0
	DefaultHandler,  // Analog Comparator 1
	DefaultHandler,  // Analog Comparator 2
	DefaultHandler,  // System Control (PLL, OSC, BO)
	DefaultHandler,  // FLASH Control
	DefaultHandler,  // GPIO Port F
	DefaultHandler,  // GPIO Port G
	DefaultHandler,  // GPIO Port H
	DefaultHandler,  // UART2 Rx and Tx
	DefaultHandler,  // SSI1 Rx and Tx
	DefaultHandler,  // Timer 3 subtimer A
	DefaultHandler,  // Timer 3 subtimer B
	DefaultHandler,  // I2C1 Master and Slave
	DefaultHandler,  // Quadrature Encoder 1
	DefaultHandler,  // CAN 0
	DefaultHandler,  // CAN 1
	DefaultHandler,  // CAN 2
	DefaultHandler,  // Ethernet
	DefaultHandler   // Hibernate
};




//Reset Handler
__attribute__ ((section(".init")))
__attribute__ ((naked))
void ResetHandler(void)
{
	unsigned long *src,*dst;

	//set stack pointer
	__asm(	"ldr r0,=%0\n"
			"mov sp,r0\n"
			:
			: "i" (&_estack)
		);

	//set vector table address
	__asm(	"ldr r0,=%0\n"
			"ldr r1,=0xe000ed08\n"
			"str r0,[r1]\n"
			:
			: "i" (&_text)
		);

	//copy the data segment initializers from flash to RAM
	src=&_etext;
	for(dst=(unsigned long*)&_data;dst<(unsigned long*)&_edata;)
	{
		*dst++=*src++;
	}

	//zero fill the bss segment
	__asm(	"ldr r0,=_bss\n"
			"ldr r1,=_ebss\n"
			"mov r2,#0\n"
			".thumb_func\n"
			"zero_loop:\n"
			"cmp r0,r1\n"
			"it lt\n"
			"strlt r2,[r0],#4\n"
			"blt zero_loop"
		);

	//jump to app`s entry point
	__asm(	"mov r0,#0\n"
			"mov r1,#0\n"
			"ldr r2,=%0\n"
			"blx r2\n"
			:
			: "i" (main)
		);

	//dead loop,never reach this point
	while(1);
}




//NMI Handler
static void NmiHandler(void)
{
	while(1);
}

//Fault Handler
static void FaultHandler(void)
{
	while(1);
}

//Default Handler
static void DefaultHandler(void)
{
	while(1);
}
