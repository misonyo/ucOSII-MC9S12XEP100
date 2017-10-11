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
					do{
						RevRes=RS232_PollComport(COM3,(unsigned char *)&rchr,1);
					}while(RevRes == 0);
					return 1;
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
int get_cmd_res(char* buff)
{
	char RevBuff[256];
	char *PTemp,*str;
	int RevRes,i=0,doLength=0;
	str="lj $ ";
	Sleep(100);
	do{
		RevRes=RS232_PollComport(COM3,(unsigned char *)RevBuff,160);
		if(RevRes>0)
		{
			RevBuff[RevRes]=0;
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
				buff[i]=RevBuff[i];
			}
		}
	}while(RevRes < 0);
	return doLength;
}
void FileWrite(int address)
{
	FILE *PFilewrite;
	int addr;
	char SnpBuff[512];
	char SendBuff[SEND_BUFF_SIZE];
	int FileLen,EraseLen,ReadLen,SnpLen;
	addr=address;
	PFilewrite=fopen("./origin.txt","rb");
	assert(PFilewrite != NULL);
	fseek(PFilewrite,0L,SEEK_END);
	FileLen=ftell(PFilewrite);
	fseek(PFilewrite,0L,SEEK_SET);
	EraseLen=((FileLen+256-1)/256)*256;
	SnpLen=snprintf(SendBuff,SEND_BUFF_SIZE,"derase %06X %02X\n",addr,(unsigned int)EraseLen);
	send_cmd_byte_lj(SendBuff,SnpLen);
	get_cmd_res(SendBuff);
	do{
		ReadLen=fread(SendBuff,1,24,PFilewrite);
		if(ReadLen > 0)
		{
			SnpLen=snp(SendBuff,SnpBuff,addr,ReadLen,sizeof(SnpBuff));
			send_cmd_byte_lj(SnpBuff,SnpLen);
			get_cmd_res(SendBuff);
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
	PFileRead=fopen("./readback.txt","wb");
	assert(PFileRead != NULL);
	ReadLen=ReadLength;
	addr=address;
	while(ReadLen > 0)
	{
		DoReadLen=(ReadLen > 32) ? 32 : ReadLen;
		SnpLen=snprintf(SendBuff,SEND_BUFF_SIZE,"dread %06X %02X\n",addr,DoReadLen);
		SendRes=send_cmd_byte_lj(SendBuff,SnpLen);
		assert(SendRes == 1);
		RevRes=get_cmd_res(RevBuff);
		PRev=RevBuff;
		PSend=SendBuff;
		if(RevRes>0)
		{
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
		}
			ReadLen-=DoReadLen;
			addr+=DoReadLen;
	}
	fclose(PFileRead);
}

int myfile_main(int argc,char*agrv[])
{
	int OpenRes;
	OpenRes=RS232_OpenComport(COM3,57600,"8N1");
	assert(OpenRes==0);

	//FileWrite(0X100000);
	FileRead(0x100000,0x720);
	return 0;
}
