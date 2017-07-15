#if 0
#include "includes.h"
OS_STK productStk[TASK_STK_SIZE];
OS_STK consumerStk[TASK_STK_SIZE];

OS_EVENT *pc_event;

void product(void * param)
{
    int *temp;
 	while(1)
 	{
 	 	temp=malloc(sizeof(int));
 	 	*temp=rand();
 	 	printf("product num:%d\r\n",*temp);
 	 	while(OS_MBOX_FULL == OSMboxPost(pc_event,temp))
 	 	{
 	 		OSTimeDly(200);
 	 	}
 	}	
}

void consumer(void *param)
{	
    int *temp;
 	INT8U err;
 	while(1)
 	{
 		temp=OSMboxPend(pc_event,0,&err);
 	    if(temp != NULL)
 	    {
	 		printf("consumer num:%d\r\n",*temp);
	 		free(temp);	
 	    }
 	}
 	
}

void product_consumer_init(void)
{
	pc_event=OSMboxCreate(NULL);
	
	OSTaskCreate(product, (void *)0, &productStk[TASK_STK_SIZE - 1], 11);
	OSTaskCreate(consumer, (void *)0, &consumerStk[TASK_STK_SIZE - 1], 12);
		
} 
#endif