
#ifndef TASK_TEST
#define TASK_TEST

#define GPIO_LED GPIOA 
#define GPIO_Pin_DEBUGLED1 GPIO_Pin_1
#define GPIO_Pin_DEBUGLED2 GPIO_Pin_2
#define PR_LED1_ON() GPIO_ResetBits(GPIO_LED,GPIO_Pin_DEBUGLED1)
#define PR_LED2_ON() GPIO_ResetBits(GPIO_LED,GPIO_Pin_DEBUGLED2)
#define PR_LED1_OFF() GPIO_SetBits(GPIO_LED,GPIO_Pin_DEBUGLED1)
#define PR_LED2_OFF() GPIO_SetBits(GPIO_LED,GPIO_Pin_DEBUGLED2)


void LED1_FLASH( void * pvParameters );
void LED2_FLASH( void * pvParameters );
void app_led1_flash(void);
void SerialTx(void * pvParameters);


#endif//TASK_TEST
