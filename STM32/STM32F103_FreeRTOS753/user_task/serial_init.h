

#ifndef SERIAL_INIT
#define SERIAL_INIT

#include "stm32f10x_it.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"



#define GPIO_USART GPIOA
#define GPIO_Pin_Tx GPIO_Pin_9
#define GPIO_Pin_Rx GPIO_Pin_10
#define GPIO_Pin_RTS GPIO_Pin_12
#define GPIO_Pin_CTS GPIO_Pin_11

#define USART1_RECV_BUF_SIZE 2048
#define USART1_SEND_BUF_SIZE 512

#define PR_BAUDRATE 115200


#define PR_SERIAL_RTS_HIGH() GPIO_SetBits(GPIO_USART,GPIO_Pin_RTS)
#define PR_SERIAL_RTS_LOW()  GPIO_ResetBits(GPIO_USART,GPIO_Pin_RTS)

typedef struct 
{
  u16 mSendNum;
  u32 mRecvNum;
  u8* mRecvBuffer;
  u8* mSendBuffer;
  u8* mRecvWritePt;
  u8* mRecvReadPt;
  u8* mSendReadPt;
  u8* mSendWritePt;

}TSerial_TypeDef;
  
extern TSerial_TypeDef g_Serial;
extern xQueueHandle xSeriaTxQueue;
extern xSemaphoreHandle xSemaphore_serial_mutex;

void APP_Serial_Init(void);
void Enable_Usart1_TxIt(void);
void Enable_Usart1_RxIt(void);
void Disable_Usart1_TxIt(void);
void Disable_Usart1_RxIt(void);
u32 APP_Serial_TxBuffer_Write(u8* buffer,u32 send_num);
void APP_USART1_ISR(void);


#endif