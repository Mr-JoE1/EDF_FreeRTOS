# EDF scheduler for FreeRTOS
  "Earliest Deadline First" scheduler implemented for FreeRTOS .
   <a href="https://www.buymeacoffee.com/mrjo3" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" width="100px" ></a>

  
# How to use :
  - Relace tasks.c file in FreeRTOS Dir.
  - Enable EDF Scheduler in your project "FreeRTOSConfig.h"
  ```C
    #define configUSE_EDF_SCHEDULER   1
  ```
  
  - Use xTaskCreatePeriodic to crease task using EDF scheduler 
    example:
     ```C
    	xTaskCreatePeriodic(
                    Button_1_Monitor,      		 /* Function that implements the task. */
                    "Button_1_Monitor",          /* Text name for the task. */
                    100,      					/* Stack size in words, not bytes. */
                    ( void * ) 0,    			/* Parameter passed into the task. */
                    1 ,							/* Priority at which the task is created. */
                    &btn1_Handler,				 /* Used to pass out the created task's handle. */
		           Btn1_TASK_PERIOD);			 /*Used t0 pass the period of the task*/
     ```

# Test Case 
  - You can easily find my modfication to tasks.c file by searching for @EDF tag
  ```C
     /****************************************
        * @EDF: New task creating function for EDF  
        */
  ```
  - In "testEDF.c" file you will find testing EDF app to schedule 6 tasks .
  - system tested on PLC2129xx uC using Keil IDE 
  ![Test Img](https://github.com/Mr-JoE1/EDF_FreeRTOS/blob/main/plots.png)
  
  - check "Report.pdf" for :
    - System schedulability using Rate Monotonic , Time Demand Test
    
