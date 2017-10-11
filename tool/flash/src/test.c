#include <stdio.h>
#include <string.h>
#include <conio.h>

int test_main(int argc,char*agrv[])
{
	int a=122;
	putchar(a);
	return 0;
}

#if 0
void FileRead(void)
{
	char SendBuff[SEND_BUFF_SIZE];
	int SnpLenth,WriteLenth,i,j,count=0,flag1=0,flag2=0;
	char RevBuff[REV_BUFF_SIZE];
	int RevRes,SendRes,ReadSize;
	char *Pchar,*PBuff;
	FILE *PFileRead;
	PFileRead=fopen("./wangfan.txt","wb");
	assert(PFileRead != NULL);
	SnpLenth=snprintf(SendBuff,SEND_BUFF_SIZE,"dread 100000 720\n");
	SendRes=RS232_SendBuf(COM3,(unsigned char*)SendBuff,SnpLenth);
	assert(SendRes == SnpLenth);
	Sleep(200);
	do
	{
		ReadSize= (flag1 == 0) ? 149 : 131;
		memset(RevBuff,0,160);
		RevRes=RS232_PollComport(COM3,(unsigned char*)RevBuff,ReadSize);
		if(RevRes>0)
		{
			Pchar=RevBuff;
			PBuff=SendBuff;
			if(RevBuff[RevRes-1] != '\n')
			{
				//printf("count=%d,RevRes=%d,the last char=%d\n",count,RevRes,RevBuff[RevRes-1]);
			}
			memset(SendBuff,0,40);
			if(flag1==0)
			{
				while(*Pchar != '\n')
				{
					Pchar++;
				}
				flag1=1;
				Pchar++;
			}
			if((RevRes == 131) || (RevRes == 149))
			{
				j=32;
			}
			else
			{
				j=(RevRes-3)/4;
			}
			for(i=0;i<j;i++)
			{
				SendRes=sscanf(Pchar,"%02X ",(PBuff+i));
				Pchar+=3;
			}
			WriteLenth=fwrite(SendBuff,1,j,PFileRead);
		}
		Sleep(100);
	}while(RevRes>0);
	fclose(PFileRead);
}
#endif
