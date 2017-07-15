#if 0
#include "includes.h"
OS_STK productStk[TASK_STK_SIZE];
OS_STK consumerStk[TASK_STK_SIZE];
void *pnum[5];

OS_EVENT *pc_event;

void product(void * param)
{
    INT8U *temp;
 	while(1)
 	{
 	 	temp=malloc(sizeof(INT8U));
 	 	*temp=rand();
 	 	printf("product num:%d\r\n",*temp);
 	 	while(OS_Q_FULL == OSQPost(pc_event,temp))
 	 	{
 	 		OSTimeDly(200);
 	 	}
 	}	
}

void consumer(void *param)
{	
    INT8U *temp;
 	INT8U err;
 	while(1)
 	{
 		temp=OSQPend(pc_event,0,&err);
 	    if(temp != NULL)
 	    {
	 		printf("consumer num:%d\r\n",*temp);
	 		free(temp);	
 	    }
 	}
 	
}

void product_consumer_init(void)
{
	pc_event=OSQCreate(&pnum[0],5);
	if(pc_event != NULL)
	{
		printf("the queue created successful!\r\n");
	}
	else
	{
		printf("the queue created failed!\r\n");	
	}
	
	OSTaskCreate(product, (void *)0, &productStk[TASK_STK_SIZE - 1], 11);
	OSTaskCreate(consumer, (void *)0, &consumerStk[TASK_STK_SIZE - 1], 12);
		
} 
#endif