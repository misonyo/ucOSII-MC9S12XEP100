#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "shell.h"
#include "cmd.h"
#include "eval.h"
#include "LED.h"
#include "flash.h"
static char lasc[13];

static uint8_t ctoi(char ch)	/*char to integer*/
{
    if(isdigit(ch))
		return ch - 48;
    else if((ch >='A') && (ch <='F') || (ch >='a') && (ch <='f') )
       return isupper(ch) ? ch - 55 : ch - 87;
    else
    {
    	printf("the char is invalid!\n");
    	return 0;
    }   	   	
}

static uint16_t atoiM(char *str) 	/*char string to integer*/
{
	uint16_t len;
    uint16_t value = 0;
    uint16_t i;
    len = strlen(str);
    for (i=0; i<len; i++)
    {
        value=value*10+ctoi( *(str + i) );
    }
    return value;	
}
static uint32_t atoh(char *str)	/*hex string to hex integer*/
{
	uint32_t len;
    uint32_t value = 0;
    uint32_t i;
    len = strlen(str);
    for (i=0; i<len; i++)
    {
        value=value*16+ctoi( *(str + i) );
    }
    return value;	
}
static char* htoa(int32_t value)	/*hex integer to hex string*/
{
    int32_t i;
    int sign = 0;
    int32_t absv;    
    if(value < 0)
    {
    	sign = 1;
    	absv = -value;
    }
    else
    {
    	sign = 0;
    	absv = value;    	
    }   
    for(i=11;i>0;i--)
    {
    	if(absv%16 >= 10)
    	{
    	 	lasc[i] = 55 + (absv%16);
    	 	absv = absv/16;
    	}		
    	else
    	{
    	 	lasc[i] = 48 + (absv%16);
    	 	absv = absv/16;
    	}
    	if(0 == absv) break;
    }    
    if(sign)
    {
    	lasc[--i] = '-';
    }
    
    lasc[12] = '\0';
	return &lasc[i];
}
static char* ltoa(int32_t v)
{
    int32_t i;
    int sign = 0;
    uint32_t absv;    
    if(v < 0)
    {
    	sign = 1;
    	absv = -v;
    }
    else
    {
    	sign = 0;
    	absv = v;    	
    }   
    for(i=11;i>0;i--)
    {
    	lasc[i] = '0' + (absv%10);
    	absv = absv/10;   	
    	if(0 == absv) break;
    }    
    if(sign)
    {
    	lasc[--i] = '-';
    }
    
    lasc[12] = '\0';
	return &lasc[i];
}

static void evalFunc(int argc, char *argv[] )
 {
	char *expr = argv[1];
    int32_t result;
	result = eval(&expr);
	expr = ltoa(result);
	puts(expr);
}
static ShellCmdT evalInfo=
 {
		evalFunc,
		2,2,
		"eval",
		"1:eval 2:eval expression",
		"evaluate a expression\n",
		{NULL,NULL}
};

static void ledFunc(int argc, char *argv[] ) 
{
	char *str1= argv[1];
	char *str2=argv[2];
	int8_t ledNumber;
	int8_t ledStatus;
	ledNumber=*str1-'0';
	ledStatus=*str2-'0';
	led_status_change(ledNumber,ledStatus);
}
static ShellCmdT ledInfo= {
		ledFunc,
		2,3,
		"led",
		"led controll",
		"1:led 2:led number 3:led status\n",
		{NULL,NULL}
};

static void dEraseFunc(int argc, char *argv[] ) 
{
	DFlash_erase(atoh(argv[1]));		 	
}
static ShellCmdT dEraseInfo= {
		dEraseFunc,
		2,2,
		"derase",
		"D-Flash erase",
		"erase a D-Flash sector(256byte)\n",
		{NULL,NULL}
};

static void dWriteFunc(int argc, char *argv[] ) 
{
	uint16_t buffer[24];
	uint8_t i;
	uint8_t count;
	count=argc-2;
	for(i=0;i<count;i++)
	{	
		buffer[i]=atoh(argv[2+i]);
	}
	DFlash_write(atoh(argv[1]),buffer,count);		 	
}
static ShellCmdT dWriteInfo= {
		dWriteFunc,
		3,25,
		"dwrite",
		"Write data to a D-Flash sector(256byte)",
		"1:dwrite 2:address 3...:writed data\n",
		{NULL,NULL}
};

static void dReadFunc(int argc, char *argv[] ) 
{
	char *str;
	uint8_t i; 
	uint16_t* pReadData;
	uint8_t count;
	count=(uint8_t)(atoiM(argv[2]));
	pReadData=DFlash_Read(atoh(argv[1]),count);
	for(i=0;i<count;i++)
	{
		str = htoa(*pReadData);
		if((0 == (i % 8)) && (i>0))
		{
			printf("\n");
		}
		printf("%s ",str);
		pReadData++;
		 	
	}
	printf("\n");
}
static ShellCmdT dReadInfo= {
		dReadFunc,
		2,10,
		"dread",
		"read data from a D-Flash sector",
		"1:dread 2:address 3:read count\n",
		{NULL,NULL}
};



void Cmd_Init(void)
{
	SHELL_AddCmd(&evalInfo);
	SHELL_AddCmd(&ledInfo);
	SHELL_AddCmd(&dEraseInfo);
	SHELL_AddCmd(&dWriteInfo);
	SHELL_AddCmd(&dReadInfo);
}