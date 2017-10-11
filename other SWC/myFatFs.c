#include "ff.h"
#include <stdio.h>
#include <string.h>
static FATFS FatFs;
FIL fil;

void myFatFsRead(char *FileName)
{
	char buffer[64];
	UINT len=0;
	f_open(&fil,FileName,FA_READ);
	f_read(&fil,buffer,sizeof(buffer)-1,&len);
	buffer[len] = 0;
	printf(">> FatFS test read: '%s'\n",buffer);
	f_close(&fil);
}

void myFatFsWrite(char *FileName)
{
	UINT len=0;
	UINT WriteRes=22;
	char* str="this is a test string\n";
	f_open(&fil,FileName,FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	WriteRes=f_write(&fil,str,strlen(str),&len);
	printf("write result= %d\n",WriteRes);
	f_close(&fil);
}

 void myFatFs(void)
 {

	f_mount(&FatFs, "", 1);
	myFatFsRead("/read.txt");
	myFatFsWrite("/write.txt");
	myFatFsRead("/write.txt");
	
 }
