#include "stdint.h"
#include "stdbool.h"

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/fpu.h"

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "udf_delay.h"
#include "udf_uart.h"

//--------------------DO NOT DELETE THIS FUNCTOIN------------------------------------------
void SystemInit(void)
{
	// this function set system clock to 50MHz
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |  SYSCTL_OSC_MAIN);
	//enable fpu
	FPUStackingEnable();
	FPUEnable();
	//Config UART0 8-N-1 115200 Use PA0 PA1
	Uart0Config();
	printf("System Init OK!...\r\n");
}
//--------------------END------------------------------------------------------------------

void LEDConfig(void)
{
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); 
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,(GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3));
}

void LEDFlash(void)
{
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0); 
  		DelayMs(500); 
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0xFF); 
  		DelayMs(500);
}


int main(void)
{   
		float fTemp=3.14;
		LEDConfig();
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);  
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,GPIO_PIN_4); 
		GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_4,0xff);
    while(1)
	   {
				LEDFlash();
//				printf("%7.4f\r\n",fTemp);
//				fTemp=fTemp+1.0;
		 }
//	return 0;
}
