#ifndef _TIMER_H_
#define _TIMER_H_

#include "stm32f10x.h"

typedef struct
{
  //u32 mSysTick10ms;
  u8  m20msFlag;
  u8  m500msFlag;
  u8  m1minFlag;
  u32 mSysTickle;
}sSysTick_TypeDef;

void pr_SysTick_Init(void);
void pr_SysTick_Isr(void);
extern sSysTick_TypeDef sSysTick;


#endif
