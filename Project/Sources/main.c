#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "includes.h"
#include "shell.h"
#include "flash.h"

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
OS_STK        TaskStartStk[TASK_STK_SIZE];
OS_STK        TaskEventStk[TASK_STK_SIZE];
OS_STK        TaskShellStk[TASK_STK_SIZE];

OS_FLAG_GRP* TaskEventFlag;
OS_EVENT *mbox;

/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskEvent (void *pdata)
{
	OS_FLAGS flag;
	INT8U err;
	for(;;)
	{
		flag = OSFlagPend(TaskEventFlag,0xFFFF,OS_FLAG_WAIT_SET_ANY,0,&err);

		OSFlagPost(TaskEventFlag,flag,OS_FLAG_CLR,&err);
		key_task(flag);
	}
}

/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/

void  TaskStart (void *pdata)
{
    INT8U err;
    tFlashParam FlashInit;
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif

    pdata = pdata;                                         /* Prevent compiler warning                 */

    FlashInit.patchlevel=0x00;
    FlashInit.minornumber=0x01;
    FlashInit.majornumber=0x01;
    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    TaskEventFlag =  OSFlagCreate(0,&err);
    
    OSTaskCreate(TaskEvent, (void *)0, &TaskEventStk[TASK_STK_SIZE - 1], 0);
    
    led_init();
    ShuMaGuan_init();
    key_init();
    atd_init();
    pwm_init();
    sci_init();
 	SHELL_Init();
    Cmd_Init();
    DFlashInit(&FlashInit);
    myFatFs();
    
    OSTaskCreate(SHELL_Mainloop, (void *)0, &TaskShellStk[TASK_STK_SIZE - 1], 1);
     
    EnableInterrupts;
    				
  	printf("Hello World!\r\n");
    for (;;)
    {   /* 20ms periodic task */
    	led_periodic_task();
		OSTimeDly(5);	
    }
}

void OSTaskIdleHook(void)
{
    /*
    static uint16_t i = 0;
 	sci_printf();
 	printf("Hello World %d times\r\n",i++);
 	*/
}

void main(void)
 {
    pll_init();
    OSInit();                                              /* Initialize uC/OS-II */

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 5);
 	
    OSStart();   
}
