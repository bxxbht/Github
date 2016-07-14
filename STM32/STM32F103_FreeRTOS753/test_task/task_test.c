
#include "FreeRTOS.h"
#include "task_test.h"
#include "task.h"
#include "stm32f10x.h"
#include "serial_init.h"
#include "semphr.h"
#include "macros.h"


void LED1_FLASH( void * pvParameters )
{
    unsigned portBASE_TYPE uxFLAG;
	( void ) pvParameters;// The parameters are not used.
	for(;;)
	{
        vTaskDelay(1);
		if(uxFLAG >=1)
        {
             PR_LED1_ON();
             uxFLAG = 0;
        }
        else
        {
            PR_LED1_OFF();
            uxFLAG = 1;
        }
#ifdef semphore_mutex_test
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.	
        //if( xSemaphoreTake( xSemaphore_serial_mutex, ( portTickType ) 10 ) == pdTRUE )
        {// We were able to obtain the semaphore and can now access theshared resource.
            APP_Serial_TxBuffer_Write("LED1_FLASH\n",11);
          // We have finished accessing the shared resource.  Release the semaphore.
            //xSemaphoreGive( xSemaphore_serial_mutex );
        }
        //else
        {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
        }
#endif        
	}
}

void LED2_FLASH( void * pvParameters )
{
    unsigned portBASE_TYPE uxFLAG;

	( void ) pvParameters;// The parameters are not used.

	for(;;)
	{
		vTaskDelay(500);
		if(uxFLAG >=1)
        {
             PR_LED2_ON();
             uxFLAG = 0;
        }
        else
        {
            PR_LED2_OFF();
            uxFLAG = 1;
        }
#ifdef semphore_mutex_test
        //if( xSemaphoreTake( xSemaphore_serial_mutex, ( portTickType ) 10 ) == pdTRUE )
        {// We were able to obtain the semaphore and can now access theshared resource.
            APP_Serial_TxBuffer_Write("LED2_FLASH\n",11);
          // We have finished accessing the shared resource.  Release the semaphore.
            //xSemaphoreGive( xSemaphore_serial_mutex );
        }
        //else
        {
            // We could not obtain the semaphore and can therefore not access
            // the shared resource safely.
        }
#endif
	}
}

//this function is callea by application,not task.
unsigned char g_led_flash_flag;
void app_led1_flash(void)
{
     if(g_led_flash_flag)
     {
         g_led_flash_flag=0;
         PR_LED1_ON();
     }
     else
     {
         g_led_flash_flag=1;
         PR_LED1_OFF();
     }    
}

void SerialTx(void * pvParameters)
{
    ( void ) pvParameters;// The parameters are not used.
    for(;;)
    {
        vTaskDelay(3);
        if( g_Serial.mSendNum >0)
        {
           Enable_Usart1_TxIt();
        }
   }
}





