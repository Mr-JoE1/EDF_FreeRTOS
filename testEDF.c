
/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "queue.h"
/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )
/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/************** Periodicity of the Tasks************************/
#define Btn1_TASK_PERIOD				50
#define Btn2_TASK_PERIOD				50
#define uartTX_TASK_PERIOD				100
#define uartRX_TASK_PERIOD				20
#define LOAD1_TASK_PERIOD				10
#define LOAD2_TASK_PERIOD				100

#define NULL_PTR 	(void *) 0

/* Task run time stats variables */
TaskStatics_t btn1 , btn2 , TX, RX , load1, load2 ; 

int system_time = 0;
int cpu_load = 0;

/* buf for storing  run time stats*/
char runTimeStatesBuff[200];

/* Tasks Handlers */
TaskHandle_t btn1_Handler = NULL;
TaskHandle_t btn2_Handler = NULL;
TaskHandle_t uartTX_Handler = NULL;
TaskHandle_t uartRX_Handler = NULL;
TaskHandle_t Load1_Handler = NULL;
TaskHandle_t Load2_Handler = NULL;
QueueHandle_t xQueueMsg = NULL;



static void prvSetupHardware( void );

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
*/


void Button_1_Monitor(void * pvParameters)
{
	volatile TickType_t  prevTime = xTaskGetTickCount();
	char* btn1_msg = NULL;	


	pinState_t previous_state = PIN_IS_LOW , current_state = PIN_IS_LOW;  
	
	vTaskSetApplicationTaskTag(NULL,(void *) 2);  
	
	for(;;)
	{
		current_state = GPIO_read(PORT_0,PIN0); 
		if (previous_state != current_state )
		{
			if (current_state == PIN_IS_LOW)
			{
				btn1_msg = "Btn1: Falling Edge\n";
				xQueueSend(xQueueMsg, (void *) &btn1_msg, ( TickType_t ) 0 );   
			}
			else
			{
				btn1_msg = "Btn1: Rising Edge\n";
				xQueueSend(xQueueMsg, (void *) &btn1_msg, ( TickType_t ) 0 );   
			}
			
			previous_state = current_state;
		}
		vTaskDelayUntil(&prevTime,Btn1_TASK_PERIOD);
	}
}



void Button_2_Monitor(void * pvParameters)
{
	volatile TickType_t  prevTime = xTaskGetTickCount();
	char* btn2_msg = NULL; 

	pinState_t previous_state = PIN_IS_LOW , current_state = PIN_IS_LOW; 
	
	vTaskSetApplicationTaskTag(NULL,(void *) 3);  

	for(;;)
	{
		current_state = GPIO_read(PORT_0,PIN1); 
		if (previous_state != current_state )
		{
			if (current_state == PIN_IS_LOW)
			{
				btn2_msg = "Btn2: Falling Edge\n";
				xQueueSend(xQueueMsg, (void *) &btn2_msg, ( TickType_t ) 0 );   
			}
			else
			{
				btn2_msg = "Btn2: Rising Edge\n";
				xQueueSend(xQueueMsg, (void *) &btn2_msg, ( TickType_t ) 0 );   
			}
			
			previous_state = current_state;
		}
		vTaskDelayUntil(&prevTime,Btn2_TASK_PERIOD);
	}
}





void Periodic_Transmitter( void * pvParameters)
{
	volatile TickType_t  prevTime = xTaskGetTickCount();
	
	char* uartTX_msg = NULL; 
	vTaskSetApplicationTaskTag(NULL,(void *) 4);  

	for(;;)
	{
		if ( ( uxQueueSpacesAvailable( xQueueMsg ) ) > 0 && (xQueueMsg != NULL) )
		{
			uartTX_msg = "uartTX_msg\n";
			xQueueSend(xQueueMsg, (void *) &uartTX_msg, ( TickType_t ) 0 );   
		}
		vTaskDelayUntil(&prevTime,uartTX_TASK_PERIOD);
	}

}


void Uart_Receiver( void * pvParameters)
{
	volatile TickType_t  prevTime = xTaskGetTickCount();
	char* uartRX_msg = NULL;
	vTaskSetApplicationTaskTag(NULL,(void *) 5);  

	for(;;)
	{
		if( xQueueReceive( xQueueMsg, &( uartRX_msg ), ( TickType_t ) 0 ) == pdPASS )
		{ 
			vSerialPutString(uartRX_msg,20); 
		}
		vTaskDelayUntil(&prevTime,uartRX_TASK_PERIOD);
	}
}


void Load_1_Simulation( void * pvParameters )
{
	int i;

	volatile TickType_t  prevTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag(NULL,(void *) 6);
    for( ;; )
    {
        /* Task code goes here. */
			
			for (i = 0; i < 37500; i++)
			{
			}
			
			vTaskDelayUntil(&prevTime,LOAD1_TASK_PERIOD);
			
			/* USED to print over uart system run time states using 
				freeRTOS API

			vTaskGetRunTimeStats(  runTimeStatesBuff ;
			
			vSerialPutString(runTimeStatesBuff, 200); 

			*/
			
    } 


}

void Load_2_Simulation( void * pvParameters )
{
	int i;

	volatile TickType_t  prevTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag(NULL,(void *) 7);
    for( ;; )
    {
        /* Task code goes here. */	
			for ( i = 0; i < 89500; i++)
			{
				
			}
	
			vTaskDelayUntil(&prevTime,LOAD2_TASK_PERIOD);
    } 


}

/*Implement Tick and Idle Hooks */

void vApplicationTickHook(void)
{
	GPIO_write(PORT_1, PIN0, PIN_IS_HIGH);
	GPIO_write(PORT_1, PIN0, PIN_IS_LOW);
}


void vApplicationIdleHook(void)
{
	vTaskSetApplicationTaskTag(NULL,(void *) 8);  
}

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	/*Creating the Queue */						
	xQueueMsg = xQueueCreate( 3, sizeof( unsigned char[20] ) );

    /* Create Tasks here */
	xTaskCreatePeriodic(
                    Button_1_Monitor,      		 /* Function that implements the task. */
                    "Button_1_Monitor",          /* Text name for the task. */
                    100,      					/* Stack size in words, not bytes. */
                    ( void * ) 0,    			/* Parameter passed into the task. */
                    1 ,							/* Priority at which the task is created. */
                    &btn1_Handler,				 /* Used to pass out the created task's handle. */
					Btn1_TASK_PERIOD);			 /*Used t0 pass the period of the task*/


	xTaskCreatePeriodic(
                    Button_2_Monitor,      
                    "Button_2_Monitor",         
                    100,      
                    ( void * ) 0,    
                    1 ,
                    &btn2_Handler, 
					Btn2_TASK_PERIOD); 


	xTaskCreatePeriodic(
                    Periodic_Transmitter,     
                    "Periodic_Transmitter",          
                    100,     
                    ( void * ) 0,   
                    1 ,
                    &uartTX_Handler, 
					uartTX_TASK_PERIOD);    
										
										
										
	xTaskCreatePeriodic(
                    Uart_Receiver,     
                    "Load_1_Simulation",         
                    100,      
                    ( void * ) 0,   
                    1 ,
                    &uartRX_Handler, 
					uartRX_TASK_PERIOD);     
	
	 xTaskCreatePeriodic(
                    Load_1_Simulation,      
                    "Load_1_Simulation",         
                    100,      
                    ( void * ) 0,   
                    1 ,
                    &Load1_Handler, 
					LOAD1_TASK_PERIOD);     
										
	xTaskCreatePeriodic(
				Load_2_Simulation,       
				"Load_2_Simulation",        
				100,     
				( void * ) 0,    
				1 ,
				&Load2_Handler,
				LOAD2_TASK_PERIOD); 
								
										
	

	vTaskStartScheduler();

	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


