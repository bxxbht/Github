

#include "macros.h"
#include "hardware_init.h"
#include "stm32f10x_it.h"
#include "task_test.h"
/*-----------------------------------------------------------*/
//***************************************************************************
// - function: Configure STM32 System's RCC
// - function module: RCC Configure operation
// - input£ºnone
// - return:none
// - comment£ºthis function ported from STM32 official Example
// -          to use external or internal clock 
// -          please set or reset the macros_HSI_CLK_ _HSE_CLK_
// - current Configuration:
// - HCLK = 36 MHz
// - PCLK1 = 36 MHz/2 = 18 MHz
// - PCLK2 = 36 MHz
// - PLL = 36MHz
//***************************************************************************
void RCC_Configuration(void)
{
   /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  
#ifdef _HSE_CLK_ // USE EXTERNAL CLOCK
  // Enable HSE 
  RCC_HSEConfig(RCC_HSE_ON);
  
  ErrorStatus HSEStartUpStatus;
  
  //Wait till HSE is ready 
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    // Enable Prefetch Buffer 
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // Flash 2 wait state 
    FLASH_SetLatency(FLASH_Latency_2);
 	
    // HCLK = SYSCLK 72MHz
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    // PCLK2 = HCLK  
    // if APB2 Prescaler=1,TIM1_CLK = PCLK1
    // else APB2 Prescaler=OTHERS,TIM1_CLK = 2*PCLK1
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    // PCLK1 = HCLK/2 36MHz
    // if APB1 Prescaler=1,TIMx_CLK = PCLK1
    // else APB1 Prescaler=OTHERS,TIMx_CLK = 2*PCLK1
    RCC_PCLK1Config(RCC_HCLK_Div2);

    // PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    // Enable PLL 
    RCC_PLLCmd(ENABLE);

    // Wait till PLL is ready 
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    // Select PLL as system clock source 
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // Wait till PLL is used as system clock source 
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
#endif
  
#ifdef _HSI_CLK_ // USE INTERNAL CLOCK
  // 8 MHz HSI ENABLE
  RCC_HSICmd(ENABLE);
  // Wait till HSE is ready 
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {
    
  }
   if(1)
  {
   
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);
   
    //HCLK = 36 MHz
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    
    //PCLK2 = 36 MHz
    // if APB2 Prescaler=1,TIM1_CLK = PCLK2
    // else APB2 Prescaler=OTHERS,TIM1_CLK = 2*PCLK2
    RCC_PCLK2Config(RCC_HCLK_Div1);
    
    //PCLK1 = 36 MHz/2 = 18 MHz,up to 36MHz  timer4 related
    // if APB1 Prescaler=1,TIMx_CLK = PCLK1
    // else APB1 Prescaler=OTHERS,TIMx_CLK = 2*PCLK2
    RCC_PCLK1Config(RCC_HCLK_Div2);
    
    //SET PLL CLOCK SOURCE AND FREQUENCY MULTIPLIER
    // 8MHz /2 * 9 = 36 MHz ST MAXIMUM PLL FREQUENCY UPTO 36MHz
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);
  
                
    //TO ENABLE PLL 
    RCC_PLLCmd(ENABLE);//IF PLL USED AS SYS CLK,IT CAN NOT BE ENABLED
    
    //WAIT FOR PLL CLK SOURCE READY
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    //SET SYS CLK,CONFIG PLL AS SYS CLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
    //WAIT FOR PLL CONFIG AS SYS CLK SUCCESSFULLY
    //  0x00£ºHSI CONFIGED SYS CLK
    //  0x04£ºHSE CONFIGED SYS CLK
    //  0x08£ºPLL CONFIGED SYS CLK  
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
#endif
  //ENABLE TIM4 CLOCK 
  //THIS STATEMENT SHOULD BE MODIFIED IF HARDWARE CHANGED
}
//***************************************************************************
// - function: STM32'S NVIC Init;
// - function module: NVIC operation
// - input£ºnone
// - return:none
// - comment£ºthis function ported from STM32 official Example
//***************************************************************************
void NVIC_Configuration(void)
{
  #ifdef  VECT_TAB_RAM  
    /* Set the Vector Table base location at 0x20000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
  #else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
  #endif  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}
//***************************************************************************
// - function: STM32'S GPIO Main Configuration;
// - function module: GPIO Main operation
// - input£ºnone
// - return:none
// - comment£ºthis function ported from STM32 official Example
//***************************************************************************
void GPIO_Configuration(void)
{
  /*
     Configure all unused GPIO port pins in Analog Input mode (floating input
     trigger OFF), this will reduce the power consumption and increase the device
     immunity against EMI/EMC
  */
  GPIO_InitTypeDef GPIO_InitStructure;
 
  //////////////////////////////////////////////////////////////////////////////
  //--------------------------------------------------------------------------//
  //////////////////////////////////////////////////////////////////////////////
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
                         RCC_APB2Periph_GPIOG, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
                         RCC_APB2Periph_GPIOG, DISABLE);    
}

//***************************************************************************
// - function: STM32'S Debug Led Control Pin Inits;
// - function module: Debug Led Control Operation
// - input£ºnone
// - return:none
// - comment£ºTo Close and Debug Led ,use PR_Debug_LEDx_ON/OFF();
//***************************************************************************
void APP_GPIO_Led_Init(void)
{
  //-------------------------- CONFIG RCC For GPIOA --------------------------//
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
  
  
  //-------------------------- CONFIG GPIO FOR LED DEBUG --------------------------//
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_DEBUGLED1 | GPIO_Pin_DEBUGLED2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIO_LED, &GPIO_InitStructure);

}