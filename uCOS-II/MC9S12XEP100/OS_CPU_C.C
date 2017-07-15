/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                          All Rights Reserved
*
*
*                                       MC9S12DP256/DG128 Specific code
*                                          SMALL MEMORY MODEL
*
* File : OS_CPU_C.C
* By   : Jean J. Labrosse
  
*******************************************************************************************************/

#define  OS_CPU_GLOBALS
#include "includes.h"
#ifndef  CRGFLG
#define  CRGFLG (*((volatile unsigned char*)(0x0037)))
#endif


OS_STK *OSTaskStkInit(void (*task)(void *pd), void *p_arg, OS_STK *ptos, INT16U opt)
{    
    INT16U *wstk;
    INT8U  *bstk;
    
    volatile INT16U D = (INT16U)p_arg;
    
    D = ((D>>8)&0xFF) + ((D<<8)&0xFF00);

    opt     =  opt;                                             /* 'opt' is not used, prevent warning                   */
    wstk    =  (INT16U *)ptos;                                  /* Load stack pointer                                   */
    *--wstk =  (INT16U)p_arg;                                   /* Simulate call to function with argument              */
    *--wstk =  (INT16U)(((INT32U)task) >> 8);                   /* Return address of simulated call. Format: PCH:PCL    */
    *--wstk =  (INT16U)(((INT32U)task) >> 8);                   /* Put task return address on top of stack              */
    *--wstk =  (INT16U)0x2222;                                  /* Y Register                                           */
    *--wstk =  (INT16U)0x1111;                                  /* X Register                                           */
    *--wstk =  (INT16U)D;                                       /* D Register                                           */
    *--wstk =  (INT16U)0x0080;                                  /* CCR: Disable STOP, Int. Level = 0. User Mode         */
    bstk    =  (INT8U *)wstk;                                   /* Convert WORD ptr to BYTE ptr to set CCR              */
    *--bstk = *(INT8U *)0x10;                                   /* Save the GPAGE register (see note 6)                 */
    *--bstk = *(INT8U *)0x17;                                   /* Save the EPAGE register (see note 6)                 */
    *--bstk = *(INT8U *)0x16;                                   /* Save the RPAGE register (see note 6)                 */
    *--bstk =  (INT8U  )task;                                   /* Save the task's PPAGE register value                 */
    return ((OS_STK *)bstk);                                    /* Return pointer to new top-of-stack                   */
}

/*$PAGE*/
#if OS_CPU_HOOKS_EN
/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
void OSTaskCreateHook (OS_TCB *ptcb)
{
    ptcb = ptcb;                       /* Prevent compiler warning                                     */
}


/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
void OSTaskDelHook (OS_TCB *ptcb)
{
    ptcb = ptcb;                       /* Prevent compiler warning                                     */
}

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the 
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/
void OSTaskSwHook (void)
{
}

/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your 
*              application to add functionality to the statistics task.
*
* Arguments  : none
*********************************************************************************************************
*/
void OSTaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                               TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
void OSTimeTickHook (void)
{
}




void OSInitHookBegin(void)
{
}

void OSInitHookEnd(void)
{
}

void OSTCBInitHook(OS_TCB *ptcb)
{
    ptcb = ptcb;                       /* Prevent compiler warning                                     */
}


#endif
/*
;********************************************************************************************************
;                               START HIGHEST PRIORITY TASK READY-TO-RUN
;
; Description : This function is called by OSStart() to start the highest priority task that was created
;               by your application before calling OSStart().
;
; Arguments   : none
;
; Note(s)     : 1) The stack frame is assumed to look as follows:
;   
;                  OSTCBHighRdy->OSTCBStkPtr +  0       PPAGE
;                                            +  1       CCR
;                                            +  2       B
;                                            +  3       A
;                                            +  4       X (H)
;                                            +  5       X (L)
;                                            +  6       Y (H)
;                                            +  7       Y (L)
;                                            +  8       PC(H)
;                                            +  9       PC(L)
;
;               2) OSStartHighRdy() MUST:
;                      a) Call OSTaskSwHook() then,
;                      b) Set OSRunning to TRUE,
;                      c) Switch to the highest priority task by loading the stack pointer of the
;                         highest priority task into the SP register and execute an RTI instruction.
;********************************************************************************************************
*/
void	OSStartHighRdy(void)
{

	OSTaskSwHook();				// Call Hook function
	OSRunning = TRUE;
	asm{
		ldx		OSTCBCur		// Load the value in OSTCBCur or the TCB's address to x
		lds		0,x				// Load the value pointed by OSTCBCur to sp
//		ldaa	OSRunning
//		inca					// OSRunning = 1
//		staa	OSRunning
		pula                               // Get value of PPAGE register
        staa   $15                       // Store into CPU's PPAGE register                                

        pula                               // Get value of RPAGE register
        staa   $16                       // Store into CPU's RPAGE register                                

        pula                              // Get value of EPAGE register
        staa   $17                       // Store into CPU's EPAGE register                                

        pula                               // Get value of GPAGE register
        staa   $10                       // Store into CPU's GPAGE register  
		nop
		rti
		}	
	
}

/**********************************************
*       INTERRUPT LEVEL CONTEXT SWITCH
**********************************************/
/********************************************************************************************************
;                                    INTERRUPT LEVEL CONTEXT SWITCH
;
; Description : This function is called by OSIntExit() to perform a context switch to a task that has
;               been made ready-to-run by an ISR. The PPAGE register of the preempted task has already 
;               been stacked during the start of the ISR that is currently running.
;
; Arguments   : none
;
; Note(s)     : 1) The preempted task's stack frame is assumed to look as follows:
;   
;                  OSTCBHighRdy->OSTCBStkPtr +  0       PPAGE      -----  Added by the start of the current ISR (ex: OSTickISR)
;                                            +  1       CCR        -.
;                                            +  2       B            `
;                                            +  3       A             |
;                                            +  4       X (H)         |
;                                            +  5       X (L)          -  Added by the ISR automatic context save.
;                                            +  6       Y (H)         |
;                                            +  7       Y (L)         .
;                                            +  8       PC(H)        ,
;                                            +  9       PC(L)      -'
;
;               2) The stack frame of the task to resume looks as follows:
; 
;                  OSTCBHighRdy->OSTCBStkPtr +  0       PPAGE
;                                            +  1       CCR
;                                            +  2       B
;                                            +  3       A
;                                            +  4       X (H)
;                                            +  5       X (L)
;                                            +  6       Y (H)
;                                            +  7       Y (L)
;                                            +  8       PC(H)
;                                            +  9       PC(L)
;********************************************************************************************************
*/
void	OSIntCtxSw(void)
{		
	OSTaskSwHook();					    //调用接口函数 Call Hook function
	OSTCBCur = OSTCBHighRdy;		//把最高优先级的任务控制块传给当前任务控制块 Change OSTCBCur and OSPrioCur
	OSPrioCur = OSPrioHighRdy;  //把最高优先级的任务优先级数传给当前务优先级
	asm{
		ldx		OSTCBCur			      //得到新的TCB的地址 Get the new task's TCB's address
		lds		0,x					        //从当前任务控制块获得堆栈地址 Load the new task's sp to sp register from its TCB
		pula                               // Get value of PPAGE register
        staa   $15                       // Store into CPU's PPAGE register                                

        pula                               // Get value of RPAGE register
        staa   $16                       // Store into CPU's RPAGE register                                

        pula                              // Get value of EPAGE register
        staa   $17                       // Store into CPU's EPAGE register                                

        pula                               // Get value of GPAGE register
        staa   $10                       // Store into CPU's GPAGE register  
		nop
		rti
		}			
}
/*
;********************************************************************************************************
;                                       TASK LEVEL CONTEXT SWITCH
;
; Description : This function is called when a task makes a higher priority task ready-to-run. Generally
;               a software exception is used to push the task context on to the stack, however, the
;               Freescale Serial Monitor application requires use of the SWI instruction.  Therefore,
;               this function is called by the OS_TASK_SWITCH macro as a jump to sub routine which does
;               NOT automatically stack the CCR. 
;
; Arguments   : none
;
; Note(s)     : 1) Upon entry, 
;                  OSTCBCur     points to the OS_TCB of the task to suspend
;                  OSTCBHighRdy points to the OS_TCB of the task to resume
;                
;               2) The 'swi' instruction issued by the OS_TASK_SWITCH macro 
;                  has pushed the PC  and YXABCCR register on to the stack. PPAGE
;                  must be added to the stack, along with the other CPU
;                  registers in order to save the entire context of the
;                  preempted task.
;
;               3) The stack frame of the task to suspend looks as follows:
;
;                   -->  PUSH REMAINING 
;                        REGISTERS HERE.
;                        STACK GROWS TOWARD
;                        LOW MEMORY.
;
;                  SP +  0       PC(H)
;                     +  1       PC(L)
;
;               4) The stack frame of the task to resume looks as follows:
; 
;                  OSTCBHighRdy->OSTCBStkPtr +  0       PPAGE
;                                            +  1       CCR
;                                            +  2       B
;                                            +  3       A
;                                            +  4       X (H)
;                                            +  5       X (L)
;                                            +  6       Y (H)
;                                            +  7       Y (L)
;                                            +  8       PC(H)
;                                            +  9       PC(L)
;********************************************************************************************************
*/
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt 4 void	OSCtxSw(void)
{
	asm{
/*  pshy                                   // ; Manually push preempted task's context on to the stack
    pshx
    psha
    pshb    
    pshc   */
    /*these were done automatically by instruction "SWI"*/
    
	ldaa   $10                     // Get current value of GPAGE register                                
    psha                               //Push GPAGE register onto current task's stack

    ldaa   $17                       // Get current value of EPAGE register                                
    psha                               // Push EPAGE register onto current task's stack

    ldaa   $16                       // Get current value of RPAGE register                                
    psha                               //Push RPAGE register onto current task's stack

    ldaa   $15                       // Get current value of PPAGE register                                
    psha                               //Push PPAGE register onto current task's stack
    		
		ldx		OSTCBCur	         	//得到TCB的地址 Get the TCB's address
		sts		0,x				        	//将SP保存到TCB的第一个字 Save the sp to TCB's first word
		}									        //OS_EXT  OS_TCB *OSTCBCur; // Pointer to currently running TCB 
	OSTaskSwHook();				    	// 调用接口函数 Call Hook function
	OSTCBCur = OSTCBHighRdy;		// 把最高优先级的任务控制块传给当前任务控制块 Change OSTCBCur and OSPrioCur
	OSPrioCur = OSPrioHighRdy;  // 把最高优先级的任务优先级数传给当前务优先级
	asm{
		ldx		OSTCBCur		        //得到新的TCB的地址 Get the new task's TCB's address
		lds		0,x					        //从当前任务控制块获得堆栈地址 Load the new task's sp to sp register from its TCB
    	pula                               // Get value of PPAGE register
        staa   $15                       // Store into CPU's PPAGE register                                

        pula                               // Get value of RPAGE register
        staa   $16                       // Store into CPU's RPAGE register                                

        pula                              // Get value of EPAGE register
        staa   $17                       // Store into CPU's EPAGE register                                

        pula                               // Get value of GPAGE register
        staa   $10                       // Store into CPU's GPAGE register  
		nop
		rti
		}	    	
}
/*
;********************************************************************************************************
;                                           SYSTEM TICK ISR
;
; Description : This function is the ISR used to notify uC/OS-II that a system tick has occurred.  You 
;               must setup the S12XE's interrupt vector table so that an RTI  interrupt 
;               vectors to this function.
;
; Arguments   : none
;
;********************************************************************************************************
*/
interrupt 7 void OSTickISR(void)
{  
	asm{
	ldaa   $10                     // Get current value of GPAGE register                                
    psha                               //Push GPAGE register onto current task's stack

    ldaa   $17                       // Get current value of EPAGE register                                
    psha                               // Push EPAGE register onto current task's stack

    ldaa   $16                       // Get current value of RPAGE register                                
    psha                               //Push RPAGE register onto current task's stack

    ldaa   $15                       // Get current value of PPAGE register                                
    psha            
		}
	OSIntEnter();
	OS_SAVE_SP();
	CRGFLG &=0xEF;			// clear the interrupt flag
	OSTimeTick();


/*	asm cli;          
	 /*these instriction is optional*/
	OSIntExit();			// exit interrupt and task switch
	
	asm{
		pula                               // Get value of PPAGE register
        staa   $15                       // Store into CPU's PPAGE register                                

        pula                               // Get value of RPAGE register
        staa   $16                       // Store into CPU's RPAGE register                                

        pula                              // Get value of EPAGE register
        staa   $17                       // Store into CPU's EPAGE register                                

        pula                               // Get value of GPAGE register
        staa   $10                       // Store into CPU's GPAGE register  
		nop
		rti
		}
		
}
#pragma CODE_SEG DEFAULT


