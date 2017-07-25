#include "flash.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdint.h>

#define readWord(address) ((uint16_t)(*(volatile uint16_t *near)(address)))
#define DFLASH_LOWEST_START_PAGE        0x00        //定义data flash的起始页
#define DFLASH_START                    0x100000  //定义data flash的起始地址
#define DFLASH_PAGE_SIZE                0x0400      //定义data flash的大小为1K.
#define DFLASH_PAGE_WINDOW_START        0x0800      //定义data flash页面窗口的起始地址
#define DFLASH_ERASE_SECTOR_SIZE 256
#define DFLASH_END 0x1080c00

#define FlashInitOk 0xA5
#define FlashInitFailed 0x5A
#define DFLASH_IS_ERASE_ADDRESS_ALIGNED(a)  ( 0 == ((DFLASH_ERASE_SECTOR_SIZE-1)&(a)) )
#define DFLASH_IS_ERASE_ADDRESS_OK(a)  ( ((a) < DFLASH_END ) && ((a) >= DFLASH_START ))
static unsigned char FlashInitStatus=FlashInitFailed;

void DFlashInit(tFlashParam* FlashParam)
{
	if((FLASH_DRIVER_VERSION_MAJOR == FlashParam->majornumber) ||
		(FLASH_DRIVER_VERSION_MINOR == FlashParam->minornumber) ||
		(FLASH_DRIVER_VERSION_PATCH == FlashParam->patchlevel))
	{
		while(FSTAT_CCIF==0);            /*等待正在处理的FLASH操作完成 */
		FCLKDIV=0x0F;                    /*外部晶振为16M.FLASH时钟不超过1M，具体参照手册 ,Flash Clock Divider Register  */
		FCNFG=0x00;                     /*禁止中断*/
		while(FCLKDIV_FDIVLD==0);        /*等待时钟设置成功*/
		FlashParam->errorcode = kFlashOk;
		FlashInitStatus=FlashInitOk;
	}
	else
	{
		FlashParam->errorcode = kFlashFailed;
		FlashInitStatus=FlashInitFailed;
	}   
}
void DFlashDeinit(tFlashParam* FlashParam)
{
	if(FlashInitStatus == FlashInitOk)
	{
		while(FSTAT_CCIF==0);            /*等待正在处理的FLASH操作完成 */
		FCLKDIV=0x00;                    /*外部晶振为16M.FLASH时钟不超过1M，具体参照手册 ,Flash Clock Divider Register  */
		FCNFG=0x00;                     /*禁止中断*/
		while(FCLKDIV_FDIVLD==0);        /*等待时钟设置成功*/
		FlashParam->errorcode = kFlashOk;
	}
	else
	{
		FlashParam->errorcode = kFlashFailed;
	}
}
void DFlashErase(tFlashParam* FlashParam)	/*擦除DFLASH的一个分区*/
{
	tAddress address;
	tLength length;
	int i;
	uint16_t ReadData;
	uint8_t lastepage;          //用于存储EPAGE的值
	uint8_t epage;              //用于计算EPAGE的值
    lastepage = EPAGE;   //保存EPAGE的值
	address=FlashParam->address;
	length=FlashParam->length;
	if(FlashInitStatus == FlashInitOk)
	{
		if((FALSE == DFLASH_IS_ERASE_ADDRESS_ALIGNED(address)) ||
			(FALSE == DFLASH_IS_ERASE_ADDRESS_OK(address)))
		{
			FlashParam->errorcode=kFlashInvalidAddress;
		}
		else if((FALSE == DFLASH_IS_ERASE_ADDRESS_ALIGNED(length)) ||
			(FALSE == DFLASH_IS_ERASE_ADDRESS_OK(address+length)))
		{
			FlashParam->errorcode=kFlashInvalidSize;
		}
		else
		{
			while(length > 0)
			{
				while(FSTAT_CCIF==0);
				if(FSTAT_ACCERR)           //判断并清除标志位；
				  FSTAT_ACCERR=1;
				if(FSTAT_FPVIOL)           //判断并清除标志位；
				  FSTAT_FPVIOL=1;
				FCCOBIX_CCOBIX=0x00;
				FCCOB=0x1200 | ((address & 0x00FF0000) >>16);           //写入擦除命令和高位地址
				FCCOBIX_CCOBIX=0x01;
				FCCOB=address & 0x0000FFFF;           //写入低16位的地址
				FSTAT_CCIF=1;           //启动执行命令
				while(FSTAT_CCIF==0);   //等待执行完成
				length -= DFLASH_ERASE_SECTOR_SIZE;
				address += DFLASH_ERASE_SECTOR_SIZE;
			}
			address=FlashParam->address;
			length=FlashParam->length;
			for(i=0;i<(length/2);i++)
			{
				epage = (byte)((DFLASH_LOWEST_START_PAGE)+(address >>10));   //计算EPAGE
				EPAGE=epage;                                                     //给EPAGE赋值
				ReadData = readWord((address & (DFLASH_PAGE_SIZE - 1)) + DFLASH_PAGE_WINDOW_START);  //读取页面窗口中的数据
				if( 0xffff != ReadData)
				{
					FlashParam->errorAddress=address;
					FlashParam->errorcode = kFlashFailed;
					break;
				}
				address+=2;
			}
			EPAGE= lastepage;       //恢复EPAGE的值
			FlashParam->errorcode = kFlashOk;
		}
	}
	else
	{
		FlashParam->errorcode = kFlashFailed;
	}
}

void DFlashWrite(tFlashParam* FlashParam)	/*向DFLASH写入数据 */
{
	uint8_t doCount=0;
	uint8_t index=0,i;
	tAddress address;
	tLength length;
	tData *data;
	uint16_t ReadData;
	uint8_t lastepage;          //用于存储EPAGE的值
	uint8_t epage;              //用于计算EPAGE的值
	lastepage = EPAGE;   //保存EPAGE的值
	address=FlashParam->address;
	length=FlashParam->length;
	data=FlashParam->data;
	if(FlashInitStatus == FlashInitOk)
	{
		if((FALSE == DFLASH_IS_ERASE_ADDRESS_ALIGNED(address)) ||
			(FALSE == DFLASH_IS_ERASE_ADDRESS_OK(address)))
		{
			FlashParam->errorcode=kFlashInvalidAddress;
		}
		else if((0 != (length % 2)) ||
			(FALSE == DFLASH_IS_ERASE_ADDRESS_OK(address+length)))
		{
			FlashParam->errorcode=kFlashInvalidSize;
		}
		else if(FALSE == data)
		{
			FlashParam->errorcode=kFlashInvalidData;
		}
		else
		{
			while(length >0)
			{
				doCount=(length>8 ? 8 : length)/2;
				while(FSTAT_CCIF==0);
				if(FSTAT_ACCERR)           //判断并清除标志位；
					FSTAT_ACCERR=1;
				if(FSTAT_FPVIOL)           //判断并清除标志位；
					FSTAT_FPVIOL=1;
				FCCOBIX_CCOBIX=0x00;
				FCCOB=0x1100 | ((address & 0x00FF0000) >>16);         //写入命令和高位地址
				FCCOBIX_CCOBIX=0x01;
				FCCOB=address & 0x0000FFFF;         //写入低16位地址
				for(i=0;i<doCount;i++)
				{
					FCCOBIX_CCOBIX=0x02+i;  //写入第i个数据
					FCCOB=(uint16_t)(*data << 8); /* 保存到高8位 */
					data++;
					FCCOB |= (uint16_t)(*data); /* 保存到低8位 */
					data++;
				}
				length-=doCount*2;
				address+=doCount*2;
				FSTAT_CCIF=1;         //启动执行命令
				while(FSTAT_CCIF==0); //等待执行完毕
			}
			address=FlashParam->address;
			length=FlashParam->length;
			data=FlashParam->data;
			for(i=0;i<(length/2);i++)
			{
				epage = (byte)((DFLASH_LOWEST_START_PAGE)+(address >>10));   //计算EPAGE
				EPAGE=epage;                                                     //给EPAGE赋值
				ReadData = readWord((address & (DFLASH_PAGE_SIZE - 1)) + DFLASH_PAGE_WINDOW_START);  //读取页面窗口中的数据
				if(((ReadData >> 8) != *data)) 
				{
					FlashParam->errorAddress=address;
					FlashParam->errorcode = kFlashFailed;
					break;
				}
			    if(((ReadData << 8) != *(++data)))
				{
					FlashParam->errorAddress=address;
					FlashParam->errorcode = kFlashFailed;
					break;
				}
				address+=2;
				data++;
			}
			EPAGE= lastepage;       //恢复EPAGE的值
			FlashParam->errorcode = kFlashOk;
		}
	}
	else
	{
		FlashParam->errorcode = kFlashFailed;
	}
}
void DFlashRead(tFlashParam* FlashParam)
{
	uint8_t lastepage;          //用于存储EPAGE的值
	uint8_t epage;              //用于计算EPAGE的值
	uint8_t i;
	tAddress address;
	tLength length;
	tData *data;
	uint16_t ReadData;
	lastepage = EPAGE;   //保存EPAGE的值
	address=FlashParam->address;
	length=FlashParam->length;
	data=FlashParam->data;
	if(FlashInitStatus == FlashInitOk)
	{
		if((FALSE == DFLASH_IS_ERASE_ADDRESS_ALIGNED(address)) ||
			(FALSE == DFLASH_IS_ERASE_ADDRESS_OK(address)))
		{
			FlashParam->errorcode=kFlashInvalidAddress;
		}
		else if((0 != (length % 2)) ||
			(FALSE == DFLASH_IS_ERASE_ADDRESS_OK(address+length)))
		{
			FlashParam->errorcode=kFlashInvalidSize;
		}
		else if(FALSE == data)
		{
			FlashParam->errorcode=kFlashInvalidData;
		}
		else
		{
			for(i=0;i<length;i++)
			{
				epage = (byte)((DFLASH_LOWEST_START_PAGE)+(address >>10));   //计算EPAGE
				EPAGE=epage;                                                     //给EPAGE赋值
				ReadData= readWord((address & (DFLASH_PAGE_SIZE - 1)) + DFLASH_PAGE_WINDOW_START);  //读取页面窗口中的数据
				address+=2;
				*data=ReadData >> 8;
				data++;
				*data=ReadData << 8;
				data++;
			}
			EPAGE= lastepage;       //恢复EPAGE的值
			FlashParam->errorcode = kFlashOk;
		}
	}
	else
	{
		FlashParam->errorcode = kFlashFailed;
	}
}
