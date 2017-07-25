#include <stdint.h>
#include "includes.h"

static void key_isr_process()
{
	int8_t err;
	OS_FLAGS flag = (OS_FLAGS)PIFH;
	PIFH = (INT8U)flag;
	(void)OSFlagPost(TaskEventFlag,flag,OS_FLAG_SET,&err);
}

#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt void PORTH_INT(void)
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
	 	// check it was Key0ISR bit 0 by check REG ISR FLAG
	key_isr_process();
	
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