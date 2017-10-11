#include "includes.h"
#include "eval.h"
#include "SCI_ISR.h"

static uint8_t SendCache[1024];
static uint8_t *SendCacheIn=&SendCache[0];
static uint8_t *SendCacheOut=&SendCache[0];
static volatile uint16_t SendCacheCount=0;
static uint8_t RecvCache[1024];
static uint8_t *RecvCacheIn=&RecvCache[0];
static uint8_t *RecvCacheOut=&RecvCache[0];
static volatile uint16_t RecvCacheCount=0;

void TERMIO_PutChar(char data)
{
 #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
	while(SendCacheCount >= 1024) 
	{
		OSTimeDly(1); /* wait cache empty in case of full */
	}
	if(data == '\n')
	{
		TERMIO_PutChar('\r');
	}
    OS_ENTER_CRITICAL();
	if(SendCacheCount < 1024)
	{
		*SendCacheIn=data;
		SendCacheIn++;
		SendCacheCount++;
		if(SendCacheIn== (&SendCache[1024]))
		{
			SendCacheIn= &SendCache[0];
		}
		
		SCI0CR2_TIE = 1;	/*Transmitter Interrupt Enable*/
	}
	OS_EXIT_CRITICAL();
}

void sci_int_printf(void)
{
	if(SendCacheCount>0)
	{
		if(SCI0SR1_TDRE)	/*1 Byte transferred to transmit shift register; transmit data register empty*/
		{
			SCI0DRL =*SendCacheOut;
			SendCacheOut++;
			SendCacheCount--;
			if(SendCacheOut==(&SendCache[1024]))
			{
				SendCacheOut=&SendCache[0];	
			}
		}	
	}
	else
	{
	   	SCI0CR2_TIE = 0;
	}		
}

void sci_printf(void)
{
	if(SendCacheCount>0)
	{
		while(!SCI0SR1_TDRE);
		SCI0DRL =*SendCacheOut;
		SendCacheOut++;
		SendCacheCount--;
		if(SendCacheOut==(&SendCache[1024]))
		{
			SendCacheOut=&SendCache[0];	
		}	
	}	
}

void sci_int_receive(void)
{
	if(SCI0SR1_RDRF)	/*1:Received data available in SCI data register*/
	{
		*RecvCacheIn=SCI0DRL;
		RecvCacheIn++;
		RecvCacheCount++;
		if(RecvCacheIn==(&RecvCache[1024]))
		{
			RecvCacheIn=&(RecvCache[0]);
		}		
	}	
}
char sci_int_RecvDataDeal(void)
{
	uint8_t ch;
	while(RecvCacheCount<=0) OSTimeDly(1);
	if(RecvCacheCount>0)
	{
		ch=*RecvCacheOut;
		RecvCacheOut++;
		RecvCacheCount--;
		if(RecvCacheOut==&(RecvCache[1024]))
		{
			RecvCacheOut=&RecvCache[0];
		}		
	}
	return (char)ch;
} 
	

	
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt void SCI_INT(void)
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
	
	sci_int_printf();
	sci_int_receive();

	
	
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

