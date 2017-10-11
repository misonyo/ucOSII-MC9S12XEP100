#include "derivative.h"
#include "SD.h"
#include "INCLUDES.h"

static void delay1ms(unsigned int n) 
{
    OSTimeDly(1);
}
/*************************************************************/
/*                      初始化SPI模块                        */
/*************************************************************/
void SPI_Init(void) 
{
  DDRS    = 0xE0; /*高3位为1,1 Associated pin is configured as output.*/    
  SPI0CR2 = 0x10;	/*SS port pin with MODF feature*/
  SPI0CR1 = 0x5e;	/* 0b0101 1110,SPI is in master mode*/
  SPI0BR  = 0x45; //设置波特率为100k                  
}

/*************************************************************/
/*                    设置SPI时钟为4MHz                      */
/*************************************************************/
void SPI_4M(void) 
{ 
  SPI0BR  = 0x11; //设置波特律为4M                
}

/*************************************************************/
/*                        初始化SD卡                         */
/*************************************************************/
void SD_Init(void)
{
	SPI_Init();
	SD_deselect();
	CD_dir=0;
	WP_dir=0;
}

/*************************************************************/
/*                        清空缓冲区                         */
/*************************************************************/
void clear_buffer(byte buffer[])
{
    int i;     
    for(i=0;i<512;i++)	
		*(buffer+i)=0;
}
 
/*************************************************************/
/*                      SPI读写一个字节                      */
/*************************************************************/
byte SPI_Byte(byte value)
{
	while (!SPI0SR_SPTEF); /* 1 SPI data register empty*/
	SPI0DR = value;
	while(!(SPI0SR_SPIF)); /*1 New data copied to SPIDR.*/
	return SPI0DR;
}

/*************************************************************/
/*                       向SD卡写入命令                      */
/*************************************************************/
byte SD_send_command(byte cmd, long arg)
{
	byte a;
	byte retry=0;
	
	SPI_Byte(0xff);
	SD_select();
	
	SPI_Byte(cmd | 0x40);//分别写入命令
	SPI_Byte(arg>>24);
	SPI_Byte(arg>>16);
	SPI_Byte(arg>>8);
	SPI_Byte(arg);
	SPI_Byte(0x95);
	
	while((a = SPI_Byte(0xff)) == 0xff)//等待响应，
		if(retry++ > 10) break;//超时退出

	SD_deselect();

	return a;//返回状态值
}

/*************************************************************/
/*                       向SD卡写入命令                      */
/*************************************************************/
byte SD_Reset(void)
{
	unsigned char i;
	unsigned char retry;
	unsigned char a=0;
	retry = 0;
	do
	{
		for(i=0;i<10;i++) SPI_Byte(0xff);
		a = SD_send_command(0,0);  //发空闲命令
		delay1ms(10);
		retry++;
		if(retry>10) return 1;      //超时退出
	} while(a != 0x01);


	retry = 0;
	do
	{
		a = SD_send_command(1, 0);  //发触发命令
		delay1ms(10);
		retry++;
		if(retry>100) return 1;      //超时退出
	} while(a);
	a = SD_send_command(59, 0);   

	a = SD_send_command(16, 512);//设扇区大小512

	return 0;//正常返回
}

/*************************************************************/
/*                     由SD卡读取一个扇区                    */
/*************************************************************/
byte read_block(long sector, byte* buffer)
{
	byte a;          
	word i;
	a = SD_send_command(17, sector<<9);  //读命令 	
	if(a != 0x00) 		return a;

	SD_select();
	//等数据的开始
	while(SPI_Byte(0xff) != 0xfe);

	for(i=0; i<512; i++)              //读512个数据
	{
		*buffer++ = SPI_Byte(0xff);
	}

	SPI_Byte(0xff);              
	SPI_Byte(0xff);  	
	SD_deselect();
  	SPI_Byte(0xff);              
	return 0;
} 

/*************************************************************/
/*                     向SD卡写入一个扇区                    */
/*************************************************************/
byte write_block(long sector, byte* buffer)
{
	byte a;
	word i;
  if(sector<1) return 0xff;     //为了保护SD卡引导区，跳过该区
	a = SD_send_command(24, sector<<9);//写命令
	if(a != 0x00) return a;

	SD_select();
	
	SPI_Byte(0xff);
	SPI_Byte(0xff);
	SPI_Byte(0xff);

	SPI_Byte(0xfe);//发开始符
	
	for(i=0; i<512; i++)//送512字节数据
	{
		SPI_Byte(*buffer++);
	}
	
	SPI_Byte(0xff);
	SPI_Byte(0xff);
	
	a = SPI_Byte(0xff); 	
	if( (a&0x1f) != 0x05)
	{
	  SD_deselect();
		return a;
	}
	//等待操作完
	while(!SPI_Byte(0xff));

  SD_deselect();

	return 0;
} 
