#include "udf_uart.h"

#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"  // about line 30 in this file  use "GPIO_PA0_U0RX" in this lib (in line 8052)
#include "driverlib/interrupt.h"

#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"

void Uart0Config(void)
{
		//
    // Enable the peripherals used.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
		
		GPIOPinConfigure(GPIO_PA0_U0RX);
		GPIOPinConfigure(GPIO_PA1_U0TX);
	
		//
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	
		UARTClockSourceSet(UART0_BASE,UART_CLOCK_SYSTEM);
		
		UARTFIFODisable(UART0_BASE);
	
		UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	
//		Enable Interrupt
		IntEnable(INT_UART0);
		UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT);
	
		UARTEnable(UART0_BASE);
		
//		UARTCharPut(UART0_BASE,'#');//begin 
		printf("\r\nUART0 Init OK!,This is TM4C123GH6PM...\r\n");
}

int fputc(int ch, FILE *f)
{
	while(UARTBusy(UART0_BASE)==true);
	UARTCharPut(UART0_BASE,ch);
  return (ch);
}

//
//  this is the INTERRUPT handler of UART0
//
void UART0_Handler(void)
{
	uint8_t ui8CharTemp=0;
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE,ui32Status);
	ui8CharTemp=UARTCharGet(UART0_BASE);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0xff);
	printf("\r\njjUART0 Rxd a char :%c\r\n",ui8CharTemp);
}


//以下两个函数移植于STM32 目前未出现错误 2016-06-04 00:42:56

/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被UART_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*
 * 函数名：UART_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-ui32Base 串口通道，这里只用到了串口0，即UART0_BASE
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用UART_printf( UART0_BASE, "\r\n this is a demo \r\n" );
 *            		 UART_printf( UART0_BASE, "\r\n %d \r\n", i );
 *            		 UART_printf( UART0_BASE, "\r\n %s \r\n", j );
 * 注意 由于itoa函数的限制，%D输出时 最大值为99999  详细参考本文件110行 -->for (i = 10000; i > 0; i /= 10)
 */
void UART_printf(uint32_t ui32Base, uint8_t *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					while(UARTBusy(ui32Base)==true);
					UARTCharPut(ui32Base,0x0D);	
				//USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					while(UARTBusy(ui32Base)==true);
					UARTCharPut(ui32Base,0x0A);	
//				USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						while(UARTBusy(ui32Base)==true);
						UARTCharPut(ui32Base,*s);	
//						USART_SendData(USARTx,*s);
//						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						while(UARTBusy(ui32Base)==true);
						UARTCharPut(ui32Base,*s);	
//						USART_SendData(USARTx,*s);
//						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else 
		{
			while(UARTBusy(ui32Base)==true);
			UARTCharPut(ui32Base,*Data++);	
		}
//			USART_SendData(USARTx, *Data++);
//		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}

