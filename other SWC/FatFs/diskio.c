/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "INCLUDES.H"
/* Definitions of physical drive number for each drive */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 0 : default */
#define DEV_RAM		1	/* Example: Map Ramdisk to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
		stat = RES_OK;
		break;

	case DEV_USB :
		break;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
	{
		byte ret;
		SD_Init();
		do{ 
			ret = SD_Reset();			
			if(ret == 0)     //≥ı ºªØSDø®
			{ 	  
		    	SPI_4M();     
			}
			else
			{
			  OSTimeDly(1); 
			}
		} while(ret != 0);
		stat = 0;
		break;
	}
	case DEV_USB :
		break;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive number to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_PARERR;

	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
	{
		UINT i;
		byte ret=0;
		
		// do call sd read for count sectors		
		for(i=0;(i<count)&&(ret==0);i++)
		{
			ret = read_block(sector,buff+i*512);
		}
		
		if(ret == 0)
		{
			res = RES_OK;
		}
		else
		{
			res = RES_ERROR;
		}

		break;
	}

	case DEV_USB :
		break;
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive number to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_PARERR;

	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
	{
		UINT i;
		byte ret=0;
		for(i=0;(i<count)&&(ret==0);i++)
		{
			ret = write_block(sector,buff+i*512);
		}
		if(ret == 0)
		{
			res = RES_OK;
		}
		else
		{
			res = RES_ERROR;
		}
		break;
	}
	case DEV_USB :
		break;
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive number (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;
	switch (pdrv) {
	case DEV_RAM :
		break;

	case DEV_MMC :
	{
		switch (cmd) {
		case CTRL_SYNC:
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT:
		{
			*(DWORD*)buff = (DWORD)2*1024*1024*1024/512;
			res = RES_OK;
			break;
		}
		case GET_SECTOR_SIZE:
			*(DWORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE:
			*(DWORD*)buff = (DWORD)1024*1024; /* 1M */
			res = RES_OK;
			break;

		}
		break;
	}
	case DEV_USB :
		break;
	}

	return res;
}

DWORD get_fattime (void)
{
	return 0;
}