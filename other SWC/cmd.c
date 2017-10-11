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
	{
		return ch - 48;
	}
	else if((ch >='A') && (ch <='F') || (ch >='a') && (ch <='f') )
	{
	   return isupper(ch) ? ch - 55 : ch - 87;
	}
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
	tFlashParam erase;
	erase.address=atoh(argv[1]);
	erase.length=atoh(argv[2]);
	erase.errorAddress=0;
	DFlashErase(&erase);	
	if(erase.errorcode != kFlashOk)
	{
		printf("errorcode=%d,errorAddress=%d\n",erase.errorcode,erase.errorAddress);
	}	
}
static ShellCmdT dEraseInfo= {
		dEraseFunc,
		3,3,
		"derase",
		"D-Flash erase",
		"derase <address> <length(multiply 256)>\n",
		{NULL,NULL}
};

static void dWriteFunc(int argc, char *argv[] ) 
{
	tData buffer[24];
	uint8_t i;
	tFlashParam write;
	write.address=atoh(argv[1]);
	write.length=argc-2;
	write.data=buffer;
	write.errorAddress=0;
	memset(buffer,0,sizeof(buffer));
	for(i=0;i<write.length;i++)
	{	
		buffer[i]=(tData)atoh(argv[2+i]);
	}
	DFlashWrite(&write);
	if(write.errorcode != kFlashOk)
	{
		printf("errorcode=%d,errorAddress=%d\n",write.errorcode,write.errorAddress);
	}	
}
static ShellCmdT dWriteInfo= {
		dWriteFunc,
		3,26,
		"dwrite",
		"Write data to a D-Flash sector(256byte)",
		"dwrite <address> <...:writed data>\n",
		{NULL,NULL}
};

static void dReadFunc(int argc, char *argv[] ) 
{
	uint8_t i=0;
	tData buffer[32];
	tLength doLength=0;
	tLength length;
	tAddress address;
	tFlashParam read;
	length=atoh(argv[2]);
	address=atoh(argv[1]);	
	while(length >0)
	{
		doLength=length>32 ? 32 : length;
		read.length=doLength;
		read.address=address;
    	read.data=buffer;
		DFlashRead(&read);
		for(i=0;i<doLength;i++)
		{
			printf("%02X ",(unsigned int)(*(buffer+i)));
		}
		printf("\t");
		for(i=0;i<doLength;i++)
		{
			if(isprint(*(buffer+i)))
			{
				printf("%c",*(buffer+i));
			}
			else
			{
				printf(".");
			}
		}
		printf("\n");
		length=length-doLength;
		address+=doLength;
		if(read.errorcode != kFlashOk)
		{
			printf("errorcode=%d\n",read.errorcode);
		}	
	}	
}
static ShellCmdT dReadInfo= {
		dReadFunc,
		3,10,
		"dread",
		"read data from a D-Flash sector",
		"dread <address> <read count>\n",
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
