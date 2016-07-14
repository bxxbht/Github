/*
    FreeRTOS V7.5.3 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved
*/

/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Fast Interrupt Test" - A high frequency periodic interrupt is generated
 * using a free running timer to demonstrate the use of the
 * configKERNEL_INTERRUPT_PRIORITY configuration constant.  The interrupt
 * service routine measures the number of processor clocks that occur between
 * each interrupt - and in so doing measures the jitter in the interrupt timing.
 * The maximum measured jitter time is latched in the ulMaxJitter variable, and
 * displayed on the LCD by the 'Check' task as described below.  The
 * fast interrupt is configured and handled in the timertest.c source file.
 *
 * "LCD" task - the LCD task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the LCD send the message on a queue to the LCD task instead of
 * accessing the LCD themselves.  The LCD task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" task -  This only executes every five seconds but has the highest
 * priority so is guaranteed to get processor time.  Its main function is to
 * check that all the standard demo tasks are still operational.  Should any
 * unexpected behaviour within a demo task be discovered the 'check' task will
 * write an error to the LCD (via the LCD task).  If all the demo tasks are
 * executing with their expected behaviour then the check task writes PASS
 * along with the max jitter time to the LCD (again via the LCD task), as
 * described above.
 *
 */

/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Library includes. */
#include "stm32f10x_it.h"
#include "stm32f10x_flash.h"
/* Demo app includes. */
//#include "lcd.h"
#include "LCD_Message.h"
#include "BlockQ.h"
#include "death.h"
#include "integer.h"
#include "blocktim.h"
#include "partest.h"
#include "semtest.h"
#include "PollQ.h"
#include "flash.h"
#include "comtest2.h"

/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainCOM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )

/* Constants related to the LCD. */
#define mainMAX_LINE						( 240 )
#define mainROW_INCREMENT					( 24 )
#define mainMAX_COLUMN						( 20 )
#define mainCOLUMN_START					( 319 )
#define mainCOLUMN_INCREMENT 				( 16 )

/* The maximum number of message that can be waiting for display at any one
time. */
#define mainLCD_QUEUE_SIZE					( 3 )

/* The check task uses the sprintf function so requires a little more stack. */
#define mainCHECK_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )

/* Dimensions the buffer into which the jitter time is written. */
#define mainMAX_MSG_LEN						25

/* The time between cycles of the 'check' task. */
#define mainCHECK_DELAY						( ( portTickType ) 5000 / portTICK_RATE_MS )

/* The number of nano seconds between each processor clock. */
#define mainNS_PER_CLOCK ( ( unsigned portLONG ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )

/* Baud rate used by the comtest tasks. */
#define mainCOM_TEST_BAUD_RATE		( 115200 )

/* The LED used by the comtest tasks. See the comtest.c file for more
information. */
#define mainCOM_TEST_LED			( 3 )

/*-----------------------------------------------------------*/

/*
 * Configure the clocks, GPIO and other peripherals as required by the demo.
 */
//static void prvSetupHardware( void );

/*
 * Configure the LCD as required by the demo.
 */
//static void prvConfigureLCD( void );

/*
 * The LCD is written two by more than one task so is controlled by a
 * 'gatekeeper' task.  This is the only task that is actually permitted to
 * access the LCD directly.  Other tasks wanting to display a message send
 * the message to the gatekeeper.
 */
//static void vLCDTask( void *pvParameters );

/*
 * Retargets the C library printf function to the USART.
 */
int fputc( int ch, FILE *f );

/*
 * Checks the status of all the demo tasks then prints a message to the
 * display.  The message will be either PASS - and include in brackets the
 * maximum measured jitter time (as described at the to of the file), or a
 * message that describes which of the standard demo tasks an error has been
 * discovered in.
 *
 * Messages are not written directly to the terminal, but passed to vLCDTask
 * via a queue.
 */
//static void vCheckTask( void *pvParameters );

/*
 * Configures the timers and interrupts for the fast interrupt test as
 * described at the top of this file.
 */
extern void vSetupTimerTest( void );

/*-----------------------------------------------------------*/

/* The queue used to send messages to the LCD task. */
xQueueHandle xLCDQueue;


#include "Timer.h"
#include "macros.h"
#include "task_test.h"
#include "hardware_init.h"
#include "serial_init.h"
#include "queue.h"
#include "semphr.h"


int main( void )
{

    RCC_Configuration();
    NVIC_Configuration();
    GPIO_Configuration();
    APP_GPIO_Led_Init();	
    APP_Serial_Init(); 
#ifdef serial_queue_test   
    xSeriaTxQueue = xQueueCreate( USART1_SEND_BUF_SIZE, sizeof( portCHAR ) );
    if(xSeriaTxQueue == NULL){APP_Serial_TxBuffer_Write("QueSerailCrtFail\n",17);};
#endif 
#ifdef semphore_mutex_test
    xSemaphore_serial_mutex = xSemaphoreCreateMutex();
    if(xSemaphore_serial_mutex == NULL){APP_Serial_TxBuffer_Write("SemphSerialCrtFail\n",20);};
#endif    
    xTaskCreate( LED1_FLASH, ( signed portCHAR * ) "Check", mainCHECK_TASK_STACK_SIZE, NULL, 1, NULL );	
	xTaskCreate( LED2_FLASH, ( signed portCHAR * ) "LCD", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
    xTaskCreate( SerialTx, ( signed portCHAR * ) "LCD", configMINIMAL_STACK_SIZE, NULL, 3, NULL );
    //pr_Serial_Init();
	//prvSetupHardware();

	/* Create the queue used by the LCD task.  Messages for display on the LCD
	are received via this queue. */
	//xLCDQueue = xQueueCreate( mainLCD_QUEUE_SIZE, sizeof( xLCDMessage ) );
	
	/* Start the standard demo tasks. */
	//vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
    //vCreateBlockTimeTasks();
    //vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
    //vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
    //vStartIntegerMathTasks( mainINTEGER_TASK_PRIORITY );
	//vStartLEDFlashTasks( mainFLASH_TASK_PRIORITY );
	//vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE, mainCOM_TEST_LED );

	/* Start the tasks defined within this file/specific to this demo. */
    //xTaskCreate( vCheckTask, ( signed portCHAR * ) "Check", mainCHECK_TASK_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );	
	//xTaskCreate( vLCDTask, ( signed portCHAR * ) "LCD", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

	/* The suicide tasks must be created last as they need to know how many
	tasks were running prior to their creation in order to ascertain whether
	or not the correct/expected number of tasks are running at any given time. */
    //vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );
	
	/* Configure the timers used by the fast interrupt timer test. */
	//vSetupTimerTest();
	
	/* Start the scheduler. */
	vTaskStartScheduler();
	
	/* Will only get here if there was not enough heap space to create the
	idle task. */
	return 0;
}
/*-----------------------------------------------------------*/

/*void vLCDTask( void *pvParameters )
{
xLCDMessage xMessage;

	// Initialise the LCD and display a startup message.
	prvConfigureLCD();
	LCD_DrawMonoPict( ( unsigned portLONG * ) pcBitmap );

	for( ;; )
	{
		// Wait for a message to arrive that requires displaying. 
		while( xQueueReceive( xLCDQueue, &xMessage, portMAX_DELAY ) != pdPASS );

		// Display the message.  Print each message to a different position. 
		printf( ( portCHAR const * ) xMessage.pcMessage );
	}
}*/
/*-----------------------------------------------------------*/

/*static void vCheckTask( void *pvParameters )
{
portTickType xLastExecutionTime;
xLCDMessage xMessage;
static signed portCHAR cPassMessage[ mainMAX_MSG_LEN ];
extern unsigned portSHORT usMaxJitter;

	xLastExecutionTime = xTaskGetTickCount();
	xMessage.pcMessage = cPassMessage;
	
    for( ;; )
	{
		// Perform this check every mainCHECK_DELAY milliseconds. 
		vTaskDelayUntil( &xLastExecutionTime, mainCHECK_DELAY );

		// Has an error been found in any task? 

        if( xAreBlockingQueuesStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR IN BLOCK Q\n";
		}
		else if( xAreBlockTimeTestTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR IN BLOCK TIME\n";
		}
        else if( xAreSemaphoreTasksStillRunning() != pdTRUE )
        {
            xMessage.pcMessage = "ERROR IN SEMAPHORE\n";
        }
        else if( xArePollingQueuesStillRunning() != pdTRUE )
        {
            xMessage.pcMessage = "ERROR IN POLL Q\n";
        }
        else if( xIsCreateTaskStillRunning() != pdTRUE )
        {
            xMessage.pcMessage = "ERROR IN CREATE\n";
        }
        else if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
        {
            xMessage.pcMessage = "ERROR IN MATH\n";
        }
		else if( xAreComTestTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR IN COM TEST\n";
		}				
		else
		{
			sprintf( ( portCHAR * ) cPassMessage, "PASS [%uns]\n", ( ( unsigned portLONG ) usMaxJitter ) * mainNS_PER_CLOCK );
		}

		// Send the message to the LCD gatekeeper for display. 
		xQueueSend( xLCDQueue, &xMessage, portMAX_DELAY );
	}
}*/
/*-----------------------------------------------------------*/



/*static void prvConfigureLCD( void )
{
GPIO_InitTypeDef GPIO_InitStructure;

	// Configure LCD Back Light (PA8) as output push-pull 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	// Set the Backlight Pin 
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);

	// Initialize the LCD 
	LCD_Init();

	// Set the Back Color 
	LCD_SetBackColor( White );

	// Set the Text Color 
	LCD_SetTextColor( 0x051F );

	LCD_Clear();
}*/
/*-----------------------------------------------------------*/

/*int fputc( int ch, FILE *f )
{
static unsigned portSHORT usColumn = 0, usRefColumn = mainCOLUMN_START;
static unsigned portCHAR ucLine = 0;

	if( ( usColumn == 0 ) && ( ucLine == 0 ) )
	{
		LCD_Clear();
	}

	if( ch != '\n' )
	{
		// Display one character on LCD 
		LCD_DisplayChar( ucLine, usRefColumn, (u8) ch );
		
		// Decrement the column position by 16 
		usRefColumn -= mainCOLUMN_INCREMENT;
		
		// Increment the character counter 
		usColumn++;
		if( usColumn == mainMAX_COLUMN )
		{
			ucLine += mainROW_INCREMENT;
			usRefColumn = mainCOLUMN_START;
			usColumn = 0;
		}
	}
	else
	{
		// Move back to the first column of the next line. 
		ucLine += mainROW_INCREMENT;
		usRefColumn = mainCOLUMN_START;
		usColumn = 0;	
	}

	// Wrap back to the top of the display.
	if( ucLine >= mainMAX_LINE )
	{
		ucLine = 0;
	}
	
	return ch;
}*/
/*-----------------------------------------------------------*/


