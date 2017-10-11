#if 0
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include "rs232.h"
#define COM3 2
#define SEND_BUFF_SIZE 512
#define REV_BUFF_SIZE 512

int snp(char *SendBuff,char *SnpBuff,int address,int length,int SnpLen)
{
	int i,index=0;
	index=snprintf(&SnpBuff[0],SnpLen,"dwrite %X ",address);
	for(i=0;i<length;i++)
	{
		index+=snprintf(&SnpBuff[index],SnpLen-index,"%02x ",*(SendBuff+i));
	}
	index+=snprintf(&SnpBuff[index],SnpLen-index,"\n");
	SnpBuff[index]=0;
	//printf("length = %d : ",length);
	//puts(snpBuff);
	return index;
}
void send_cmd(char* SendBuff,int SendLen)
{
	char RevBuff[4096];
	int RevRes,SendRes;
	SendRes=RS232_SendBuf(COM3,(unsigned char *)SendBuff,SendLen);
	assert(SendRes == SendLen);
	Sleep(100);
	RevRes=RS232_PollComport(COM3,(unsigned char *)RevBuff,sizeof(RevBuff));
	//printf("SendRes=%d,RevRes=%d\n",SendRes,RevRes);
	if(RevRes>0)
	{
		RevBuff[RevRes]='\0';
		puts(RevBuff);
	}
	fflush(stdout);
}

void clear_last_char(void)
{
	int RevRes,SendRes;
	char rchr;
	unsigned int bCount = 1;
	do{
		SendRes=RS232_SendByte(COM3,'\b');
		assert(SendRes == 0);
		do{
			RevRes=RS232_PollComport(COM3,(unsigned char *)&rchr,1);
		}while(RevRes == 0);

		if(rchr != '\b')
		{
			bCount ++;
		}
		else
		{
			bCount --;
		}
	} while( bCount > 0);
}
void send_cmd_byte_wf(char* SendBuff,int SendLen)
{
	char rchr;
	int RevRes,SendRes;
	char *PSend;
	int i;
	PSend=SendBuff;
	for(i=0;i<SendLen;i++)
	{
		int bSuccess = FALSE;
		do{
			SendRes=RS232_SendByte(COM3,PSend[i]);
			assert(SendRes == 0);
			do{
				RevRes=RS232_PollComport(COM3,(unsigned char *)&rchr,1);
			}while(RevRes == 0);

			if(PSend[i] == '\n')
			{
				if(rchr == '\r')
				{
					do{
						RevRes=RS232_PollComport(COM3,(unsigned char *)&rchr,1);
					}while(RevRes == 0);
				}
				else
				{
					clear_last_char();
					printf(" >> retry \\n");
					break; // skip next actionss
				}
			}

			if(rchr != PSend[i])
			{
				clear_last_char();
				printf(" >> retry '%c'\n",PSend[i]);
			}
			else
			{
				bSuccess = TRUE;
				putchar(rchr);
				fflush(stdout);
			}
		} while(FALSE==bSuccess);
	}
}
int send_cmd_byte_lj(char* SendBuff,int SendLen)
{
	char rchr;
	int RevRes,SendRes;
	char *PSend;
	int i;
	PSend=SendBuff;
	for(i=0;i<SendLen;i++)
	{
		int bSuccess = FALSE;
		do{
			SendRes=RS232_SendByte(COM3,PSend[i]);
			assert(SendRes == 0);
			do{
				RevRes=RS232_PollComport(COM3,(unsigned char *)&rchr,1);
			}while(RevRes == 0);

			if(PSend[i] == '\n')
			{
				if(rchr == '\r')
				{
					bSuccess = TRUE;
					putchar(rchr);
					//fflush(stdout);
					do{
						RevRes=RS232_PollComport(COM3,(unsigned char *)&rchr,1);
					}while(RevRes == 0);
					putchar(rchr);
					return 1;
					//fflush(stdout);
				}
				else
				{
					clear_last_char();
					printf(" >> retry '%c'\n",PSend[i]);
				}
			}
			else
			{
				if(rchr == PSend[i])
				{
					bSuccess = TRUE;
					putchar(rchr);
					//fflush(stdout);
				}
				else
				{
					clear_last_char();
					printf(" >> retry '%c'\n",PSend[i]);
				}
			}
		} while(FALSE==bSuccess);
	}
}
void get_cmd_res(void)
{
	char RevBuff[256];
	char buff[256];
	char *PRev,*Pbuff,*PTemp,*str;
	int RevRes,i=0;
	int Length,doLength;
	PRev=RevBuff;
	str="lj $ ";
	Length=160;
	Sleep(100);
	do{
		RevRes=RS232_PollComport(COM3,(unsigned char *)RevBuff,Length);
		if(RevRes>0)
		{
			RevBuff[RevRes]=0;
			printf(" >> original res =// %s //\n",RevBuff);
			PTemp=&RevBuff[RevRes-5];
			if(strcmp(PTemp,str) == 0)
			{
				doLength=RevRes-5;
			}
			else
			{
				doLength=RevRes;
			}
			for(i=0;i<doLength;i++)
			{
				buff[i]=*PRev;
				PRev++;
			}
			buff[doLength]=0;
		}
	}while(RevRes < 0);
	printf(" >> final res =// %s //\n",buff);
}
void FileWrite(int address)
{
	FILE *PFilewrite;
	int addr;
	char SnpBuff[512];
	char SendBuff[SEND_BUFF_SIZE];
	int FileLen,EraseLen,ReadLen,SnpLen;
	addr=address;
	PFilewrite=fopen("./luojiao.txt","rb");
	assert(PFilewrite != NULL);
	fseek(PFilewrite,0L,SEEK_END);
	FileLen=ftell(PFilewrite);
	fseek(PFilewrite,0L,SEEK_SET);
	EraseLen=((FileLen+256-1)/256)*256;
	SnpLen=snprintf(SendBuff,SEND_BUFF_SIZE,"derase %06X %02X\n",addr,(unsigned int)EraseLen);
	printf("FileLen=%d,SnpLen=%d\n",(int)FileLen,SnpLen);
	send_cmd(SendBuff,SnpLen);
	do{
		ReadLen=fread(SendBuff,1,24,PFilewrite);
		if(ReadLen > 0)
		{
			SnpLen=snp(SendBuff,SnpBuff,addr,ReadLen,sizeof(SnpBuff));
			//printf("ReadLen=%d,SnpLenth=%d\n",ReadLen,SnpLenth);
			send_cmd(SnpBuff,SnpLen);
			addr+=ReadLen;
		}
	} while(ReadLen > 0);
	fclose(PFilewrite);
}

void FileRead(int address,int ReadLength)
{
	char SendBuff[SEND_BUFF_SIZE];
	char RevBuff[REV_BUFF_SIZE];
	int SnpLen,WriteLen,ReadLen,DoReadLen;
	int addr,ScanfChar;
	int RevRes,SendRes,ScanfRes;
	int i;
	char *PRev,*PSend;
	FILE *PFileRead;
	PFileRead=fopen("./wangfan.txt","wb");
	assert(PFileRead != NULL);
	ReadLen=ReadLength;
	addr=address;
	while(ReadLen > 0)
	{
		DoReadLen=(ReadLen > 32) ? 32 : ReadLen;
		SnpLen=snprintf(SendBuff,SEND_BUFF_SIZE,"dread %06X %02X\n",addr,DoReadLen);
		SendRes=RS232_SendBuf(COM3,(unsigned char*)SendBuff,SnpLen);
		assert(SendRes == SnpLen);
		Sleep(100);
		RevRes=RS232_PollComport(COM3,(unsigned char*)RevBuff,160);
		PRev=RevBuff;
		PSend=SendBuff;
		memset(SendBuff,0,40);
		memset(RevBuff,0,160);
		if(RevRes>0)
		{
			while(*PRev != '\n')
			{
				PRev++;
			}
			PRev++;
		for(i=0;i<DoReadLen;i++)
			{
				ScanfRes=sscanf(PRev,"%2X ",&ScanfChar);
				if(ScanfRes != 0)
				{
					*(PSend+i)=ScanfChar;
				}
				else
				{
					printf("sscanf failed!\n");
				}
				PRev+=3;
			}
			WriteLen=fwrite(SendBuff,1,DoReadLen,PFileRead);
			ReadLen-=DoReadLen;
			addr+=DoReadLen;
		}
	}
	fclose(PFileRead);
}

int myfile_main(int argc,char*agrv[])
{
	int OpenRes,SendCmdRes;
	char str[]="eval 3+20*80\n";
	OpenRes=RS232_OpenComport(COM3,57600,"8N1");
	assert(OpenRes==0);

	//FileWrite(0X100000);
	//FileRead(0x100000,0x720);
	SendCmdRes=send_cmd_byte_lj(str,(sizeof(str)-1));
	assert(SendCmdRes == 1);
	get_cmd_res();

	return 0;
}
#endif
