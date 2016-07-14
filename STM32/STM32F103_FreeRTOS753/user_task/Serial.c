#include "Serial.h"
#include "Macros.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "string.h"
#include "Timer.h"

#define _SOFTWARE_RTS_
sSerial_TypeDef sSerial;
u8 RecvBuffer[USART1_RECV_BUF_SIZE];
u8 SendBuffer[USART1_SEND_BUF_SIZE];


/////////////////////////////////////////////////////////////////
///////////////////// Higher Level Function /////////////////////
/////////////////////////////////////////////////////////////////
//***************************************************************************
// - function: STM32'S USART1 Inits;
// - function module:USART Control Operation
// - function call: call by boot user 
// - input£ºnone
// - return:none
// - comment£º
// - author:Mianbo Huang 2012/09
//***************************************************************************
void pr_Serial_Init(void)
{
  //----------------- Init RCC FOR GPIOA and USART 1------------------//
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);
  //----------------- Init GPIO For USART ------------------//
  GPIO_InitTypeDef GPIO_InitStructure;
  //Init TX RTS Pin of USART1 as AF floating
#ifdef _SOFTWARE_RTS_
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Tx;
#else
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_RTS | GPIO_Pin_Tx;
#endif
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
   //Init RX CTS Pin of USART1 as input floating
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CTS | GPIO_Pin_Rx;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Rx;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
#ifdef _SOFTWARE_RTS_
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_RTS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
#endif
  //--------------------- Init USART1 ---------------------//
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = PR_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
#ifdef _BOOT_MAIN_DEBUG_
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
#else
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
#endif
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  // Configure USART1
  USART_Init(USART1, &USART_InitStructure);
  // Enable the USART2
  USART_Cmd(USART1, ENABLE);
  
  //--------------------- Init USART1 IT ---------------------//
  // Enable USART1 IT
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PR_USART_IRQ_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  pr_Disable_Usart1_TxIt();
  pr_Enable_Usart1_RxIt();
  
  memset(&sSerial,0,sizeof(sSerial_TypeDef));
  
  sSerial.mRecvBuffer=(u8*)RecvBuffer;
  sSerial.mSendBuffer=(u8*)SendBuffer;

  sSerial.mRecvWritePt=(u8*)RecvBuffer;
  sSerial.mRecvReadPt=(u8*)RecvBuffer;
  sSerial.mSendWritePt=(u8*)SendBuffer;
  sSerial.mSendReadPt=(u8*)SendBuffer;

  sSerial.mRecvNum=0;
  sSerial.mSendNum=0;

  memset(sSerial.mRecvBuffer,0,USART1_RECV_BUF_SIZE);
  memset(sSerial.mSendBuffer,0,USART1_SEND_BUF_SIZE);
  
}

//***************************************************************************
// - function: Write(Send) Numbers of Data into USART;
// - function module:USART Control Operation
// - function call: call by app user 
// - input£ºarray of data and number of data to be sent 
// - return:number of data sent
// - comment£º10/29/2013 bug:in this function,serial send isp will be 
// - closed,but not opened later.the serial send isp is always opened
// - by the 3ms timer isp.if tx isp closed in this function while for a short
// - time,the 3ms timer isp open the tx isp,then the read/write collision of 
// - variable sSerial.mSendNum will happen.
// - solution: when access the variable sSerial.mSendNum,close all the isp
// - author:Mianbo Huang 2012/09
//***************************************************************************
int pr_Serial_BufferWrite(u8* buffer,u32 num)
{
  //pr_Disable_Usart1_TxIt();

  if((USART1_SEND_BUF_SIZE-sSerial.mSendNum)<num)
    return 0;
  
  u32 i=0;
  for(i=0;i<num;i++)
  {
    *sSerial.mSendWritePt=*buffer;
    __disable_irq();
    sSerial.mSendNum++;
    __enable_irq();
    sSerial.mSendWritePt++;
    buffer++;
    if(sSerial.mSendWritePt>=(SendBuffer+USART1_SEND_BUF_SIZE))
      sSerial.mSendWritePt=SendBuffer;  
  }
  
  return i;
    
}
//***************************************************************************
// - function: Read numbers of data from USART into a Ram Array;
// - function module:USART Control Operation
// - function call: call by app user 
// - input£ºnone
// - return:none
// - comment£ºadd buffer over flow mechanism
// - author:Mianbo Huang 2012/09
//***************************************************************************
int pr_Serial_BufferRead(u8* buffer,u32 num)
{
  if(num>sSerial.mRecvNum)
    num=sSerial.mRecvNum;
  
  u32 i;
  for(i=0;i<num;i++)
  {
    *buffer=pr_Serial_PopRecv();
    buffer++;
  }
  
  return i;
  
}
/////////////////////////////////////////////////////////////////
///////////////////// Low Level Function ////////////////////////
/////////////////////////////////////////////////////////////////
//***************************************************************************
// - function: USART Tx Interrupt Enable;
// - function module:USART Control Operation
// - function call: call by boot/app user 
// - input£ºnone
// - return:none
// - comment£º
// - author:Mianbo Huang 2012/09
//***************************************************************************
void pr_Enable_Usart1_TxIt(void)
{
 USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}
//***************************************************************************
// - function: USART Rx Interrupt Enable;
// - function module:USART Control Operation
// - function call: call by boot/app user 
// - input£ºnone
// - return:none
// - comment£º
// - author:Mianbo Huang 2012/09
//***************************************************************************
void pr_Enable_Usart1_RxIt(void)
{
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
//***************************************************************************
// - function: USART Tx Interrupt Disable;
// - function module:USART Control Operation
// - function call: call by boot/app user 
// - input£ºnone
// - return:none
// - comment£º
// - author:Mianbo Huang 2012/09
//***************************************************************************
  void pr_Disable_Usart1_TxIt(void)
{
  USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
}
//***************************************************************************
// - function: USART Rx Interrupt Disable;
// - function module:USART Control Operation
// - function call: call by boot/app user 
// - input£ºnone
// - return:none
// - comment£º
// - author:Mianbo Huang 2012/09
//***************************************************************************
void pr_Disable_Usart1_RxIt(void)
{
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}

//***************************************************************************
// - function: Write(Send) a Single Data into USART Inits;
// - function module:USART Control Operation
// - function call: call by boot/app user 
// - input£º8 bits data to be sent
// - return:none
// - comment£ºthis is a base function of pr_Serial_BufferWrite(...)
// - 
// - author:Mianbo Huang 2012/09
//***************************************************************************
void pr_Serial_SendByte(u8 byte)
{
  
  pr_Disable_Usart1_TxIt();
  
  *sSerial.mSendWritePt=byte;
  sSerial.mSendNum++;
  sSerial.mSendWritePt++;
  if(sSerial.mSendNum>=USART1_SEND_BUF_SIZE)
    sSerial.mSendNum = USART1_SEND_BUF_SIZE;
  if(sSerial.mSendWritePt>=(SendBuffer+USART1_SEND_BUF_SIZE))
     sSerial.mSendWritePt=SendBuffer;
  
  pr_Enable_Usart1_TxIt();

}
//***************************************************************************
// - function: Remove a single byte from USART Recv Buffer Queue Inits;
// - function module:USART Control Operation
// - function call: call by boot/app user 
// - input£ºnone
// - return:data to be removed
// - comment£ºthis is a base function of pr_Serial_BufferRead(...)
//   when it handle large amount of data,it can cause problem
// - author:Mianbo Huang 2012/09
//***************************************************************************
u8   pr_Serial_PopRecv(void)
{
 
  
#if 0
  if(sSerial.mRecvNum==0)
  {
    pr_Enable_Usart1_RxIt();
    return 0xff;
  }
#endif 
  
  u8 temp;
  temp=*(sSerial.mRecvReadPt);
  
  pr_Disable_Usart1_RxIt();
  sSerial.mRecvNum--;
  sSerial.mRecvReadPt++;
  pr_Enable_Usart1_RxIt();
  
  if(sSerial.mRecvReadPt>=RecvBuffer+USART1_RECV_BUF_SIZE)
  {
    sSerial.mRecvReadPt=RecvBuffer;
  #ifdef _COM_RX_LOST_DEBUG_
    sSerial.mRecvReadPtRoundNum++;
  #endif
  }
  
  
  return temp;
  
}
//***************************************************************************
// - function:  USART Interrupt Servicing Program;
// - function module:USART Control Operation
// - function call: call only by boot user 
// - input£ºnone
// - return:none
// - comment£ºmust no send a bytes less than 2.5ms
// - author:Mianbo Huang 2012/09
//***************************************************************************
void pr_usart1_isr(void)
{
  //----------------------------------------------------------------------//
  // receive;
  
  
  
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
  {
   
    
        *(sSerial.mRecvWritePt)=USART_ReceiveData(USART1);
        
        sSerial.mRecvWritePt++;
        if(sSerial.mRecvWritePt>=(RecvBuffer+USART1_RECV_BUF_SIZE))
        {
          sSerial.mRecvWritePt=RecvBuffer;
        #ifdef _COM_RX_LOST_DEBUG_
          sSerial.mRecvWritePtRoundNum++;
        #endif
        }
        sSerial.mRecvNum++; /* Read one byte from the receive data register */
        #ifdef _COM_RX_LOST_DEBUG_
          sSerial.mTotalByte++;
        #endif
        //if(sSerial.mRecvNum>= USART1_RECV_BUF_SIZE)
         // sSerial.mRecvNum=USART1_RECV_BUF_SIZE;    
        
        if(sSerial.mRecvNum >= USART1_RECV_BUF_SIZE - 5)
        {
          PR_SERIAL_RTS_HIGH();
          pr_Serial_BufferWrite("SCM2\x0A", 5);
        }
        
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
        
  }
  //----------------------------------------------------------------------//
  // send;
  else if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET) 
  {

    //send buf is empty;
    if(sSerial.mSendNum==0)
    {
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE); 
    }//end of if(sSerial.mSendNum==0)
    else 
    {   
        USART_SendData(USART1, *sSerial.mSendReadPt);
        sSerial.mSendNum --;
        sSerial.mSendReadPt++;
        if(sSerial.mSendReadPt>=(SendBuffer+USART1_SEND_BUF_SIZE))
          sSerial.mSendReadPt=SendBuffer;
        pr_Disable_Usart1_TxIt();
    }//end of elses
    
    USART_ClearITPendingBit(USART1,USART_IT_TXE);
  }//end of else if 
  
}
