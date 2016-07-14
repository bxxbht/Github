
#include "serial_init.h"
#include "macros.h"
#include <string.h>
#include "semphr.h"

TSerial_TypeDef g_Serial;
u8 RecvBuffer[USART1_RECV_BUF_SIZE];
u8 SendBuffer[USART1_SEND_BUF_SIZE];

xQueueHandle xSeriaTxQueue;
xSemaphoreHandle xSemaphore_serial_mutex;

//***************************************************************************
// - function: STM32'S USART1 Inits;
// - function module:USART Control Operation
// - input£ºnone
// - return:none
// - comment£º
//***************************************************************************
void APP_Serial_Init(void)
{
  //----------------- Init RCC FOR GPIOA and USART 1------------------//
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);
  //----------------- Init GPIO For USART ------------------//
  GPIO_InitTypeDef GPIO_InitStructure;
  //Init TX RTS Pin of USART1 as AF floating

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Tx;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
   //Init RX CTS Pin of USART1 as input floating
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CTS | GPIO_Pin_Rx;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Rx;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_RTS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);

  //--------------------- Init USART1 ---------------------//
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = PR_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;

  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  // Configure USART1
  USART_Init(USART1, &USART_InitStructure);
  // Enable the USART1
  USART_Cmd(USART1, ENABLE);
  
  //--------------------- Init USART1 IT ---------------------//
  // Enable USART1 IT
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PR_USART_IRQ_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  Disable_Usart1_TxIt();
  Enable_Usart1_RxIt();
  
  memset(&g_Serial,0,sizeof(TSerial_TypeDef));
  
  g_Serial.mRecvBuffer=(u8*)RecvBuffer;
  g_Serial.mSendBuffer=(u8*)SendBuffer;

  g_Serial.mRecvWritePt=(u8*)RecvBuffer;
  g_Serial.mRecvReadPt=(u8*)RecvBuffer;
  g_Serial.mSendWritePt=(u8*)SendBuffer;
  g_Serial.mSendReadPt=(u8*)SendBuffer;

  g_Serial.mRecvNum=0;
  g_Serial.mSendNum=0;

  memset(g_Serial.mRecvBuffer,0,USART1_RECV_BUF_SIZE);
  memset(g_Serial.mSendBuffer,0,USART1_SEND_BUF_SIZE);
  
}


//***************************************************************************
// - function: USART Tx Interrupt Enable;
// - function module:USART Control Operation
// - input£ºnone
// - return:none
// - comment£º
//***************************************************************************
void Enable_Usart1_TxIt(void)
{
 USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}
//***************************************************************************
// - function: USART Rx Interrupt Enable;
// - function module:USART Control Operation
// - input£ºnone
// - return:none
// - comment£º
//***************************************************************************
void Enable_Usart1_RxIt(void)
{
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
//***************************************************************************
// - function: USART Tx Interrupt Disable;
// - function module:USART Control Operation
// - input£ºnone
// - return:none
// - comment£º
//***************************************************************************
void Disable_Usart1_TxIt(void)
{
  USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
}
//***************************************************************************
// - function: USART Rx Interrupt Disable;
// - function module:USART Control Operation
// - input£ºnone
// - return:none
// - comment£º
//***************************************************************************
void Disable_Usart1_RxIt(void)
{
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}

//***************************************************************************
// - function: Write(Send) Numbers of Data into USART send buffer;
// - function module:USART Control Operation
// - input£ºarray of data and number of data to be sent 
// - return:number of data sent
//***************************************************************************
u32 APP_Serial_TxBuffer_Write(u8* buffer,u32 send_num)
{
  u32 i;
   
  if((USART1_SEND_BUF_SIZE-g_Serial.mSendNum)<send_num)return 0;
  
  for( i=0;i<send_num;i++)
  {
     *g_Serial.mSendWritePt++=*buffer++;
     __disable_irq();
     g_Serial.mSendNum++;
     __enable_irq();
     
     if(g_Serial.mSendWritePt>=(SendBuffer+USART1_SEND_BUF_SIZE))
      g_Serial.mSendWritePt=SendBuffer;  
  }
  
  return i;  
}

//***************************************************************************
// - function:  USART Interrupt Servicing Program;
// - function module:USART Control Operation
// - input£ºnone
// - return:none
// - comment£ºmust no send a bytes less than 2.5ms
//***************************************************************************
void APP_USART1_ISR(void)
{
  // receive;
#ifdef serial_queue_test  
  u8 data;
#endif 
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
  {
#ifdef serial_queue_test        
        data = USART_ReceiveData(USART1);
        if(xQueueSendFromISR(  xSeriaTxQueue, &data, NULL ))//add to queue
        { }
        else
        { }
        if(xQueueIsQueueFullFromISR ( xSeriaTxQueue ))
        {
            PR_SERIAL_RTS_HIGH();
            Enable_Usart1_TxIt();
        }
#endif        
        *(g_Serial.mRecvWritePt)=USART_ReceiveData(USART1);
        
        g_Serial.mRecvWritePt++;
        if(g_Serial.mRecvWritePt>=(RecvBuffer+USART1_RECV_BUF_SIZE))
        {
           g_Serial.mRecvWritePt=RecvBuffer;
        }
        g_Serial.mRecvNum++; // Read one byte from the receive data register 
       
        if(g_Serial.mRecvNum >= USART1_RECV_BUF_SIZE - 50)
        {
            PR_SERIAL_RTS_HIGH();
        }
        
        USART_ClearITPendingBit(USART1,USART_IT_RXNE); 
  }
  // send;
  else if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET) 
  {
    
#ifdef serial_queue_test     
    if(xQueueIsQueueEmptyFromISR( xSeriaTxQueue ))
#else
    if(g_Serial.mSendNum == 0)
#endif
    {//send buf is empty;
        //USART_ITConfig(USART1, USART_IT_TXE, DISABLE); 
        Disable_Usart1_TxIt();
    }
    else 
    {   
#ifdef serial_queue_test         
      if(xQueueReceiveFromISR( xSeriaTxQueue, &data, NULL ))
        {
            USART_SendData(USART1, data);
        }
        else
        {}
#endif
        USART_SendData(USART1, *g_Serial.mSendReadPt);
        g_Serial.mSendNum --;
        g_Serial.mSendReadPt++;
        if(g_Serial.mSendReadPt>=(SendBuffer+USART1_SEND_BUF_SIZE))
          g_Serial.mSendReadPt=SendBuffer;
    }//end of elses
    
    USART_ClearITPendingBit(USART1,USART_IT_TXE);
  }//end of else if 
  
}


















































