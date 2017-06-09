#include "udf_delay.h"
#include "driverlib/sysctl.h"
#include <TM4C123.h>

void DelayMs(uint32_t time)
{
	SysCtlDelay(time * (SysCtlClockGet()/3000));
}

void DelayUs(uint32_t time)
{
	SysCtlDelay(time * (SysCtlClockGet()/3000000));
}
