
#include "Timer.h"
#include "string.h"
//#include "Macros.h"
//#include "Serial.h"
//#include "Head.h"
//#include "Paper_Temp_CHK.h"

//#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"


sSysTick_TypeDef sSysTick;

//***************************************************************************
// - function: System Tick Init
// - function module: FTP-628MCL101 Thermal Printer Module Motor Operation
// - function call: call only by boot user or app user 
// - input£ºspecified step
// - return:none
// - comment£ºthe pwm generating will be refined in its interrupt servicing program
// - author:Mianbo Huang 2012/09
//***************************************************************************

void pr_SysTick_Init(void)
{
  memset(&sSysTick,0,sizeof(sSysTick_TypeDef));

  while(SysTick_Config(72000000 / 1000));

}
//***************************************************************************
// - function: System Tick Init
// - function module: FTP-628MCL101 Thermal Printer Module Motor Operation
// - function call: call only by boot user or app user 
// - input£ºspecified step
// - return:none
// - comment£ºthe pwm generating will be refined in its interrupt servicing program
// - cost time 3.5us
// - author:Mianbo Huang 2012/09
//***************************************************************************
void pr_SysTick_Isr(void)
{
    sSysTick.mSysTickle++;
    if(sSysTick.mSysTickle%500 == 0)
    {
        sSysTick.m500msFlag = 1;
    }

}