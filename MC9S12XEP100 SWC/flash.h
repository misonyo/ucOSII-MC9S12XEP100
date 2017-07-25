#ifndef _FLASH_H_
#define _FLASH_H_

#define FLASH_DRIVER_INIT_OFFSET 0x00000004 /* offset from start address */
#define FLASH_DRIVER_DEINIT_OFFSET 0x00000006 /* offset from startaddress */
#define FLASH_DRIVER_ERASE_OFFSET 0x00000008 /* offset from start address */
#define FLASH_DRIVER_WRITE_OFFSET 0x0000000A /* offset from startaddress */
/* sa holds flash driver start address, pp contains pointer the pointer to the parameter structure */
#define FLASH_DRIVER_INIT(sa, pp) ((tFlashFct) &((sa)+FLASH_INIT_OFFSET)(pp)
#define FLASH_DRIVER_DEINIT(sa, pp) ((tFlashFct) &((sa)+FLASH_DEINIT_OFFSET)(pp)
#define FLASH_DRIVER_ERASE(sa, pp) ((tFlashFct) &((sa)+FLASH_ERASE_OFFSET)(pp)
#define FLASH_DRIVER_WRITE(sa, pp) ((tFlashFct) &((sa)+FLASH_WRITE_OFFSET)(pp)

/* access macros, sa holds flash driver start address */
#define FLASH_DRIVER_MCUTYPE(sa) ((sa)+0)
#define FLASH_DRIVER_MASKTYPE(sa) ((sa)+1)
#define FLASH_DRIVER_INTERFACE(sa) ((sa)+3)
/* hardware version information - only an example!!! */
#define FLASH_DRIVER_VERSION_MCUTYPE 0x12 /* Motorola Star12 */
#define FLASH_DRIVER_VERSION_MASKTYPE 0xab /* some mask number */
#define FLASH_DRIVER_VERSION_INTERFACE 0x01 /* interface version number */
/* software version information - only an example!!! */
#define FLASH_DRIVER_VERSION_MAJOR 0x01 /* major version number / interface */
#define FLASH_DRIVER_VERSION_MINOR 0x01 /* minor version number / internal */
#define FLASH_DRIVER_VERSION_PATCH 0x00 /* bugfix / patchlevel */

/* standard error codes */
#define kFlashOk 0x00 /* called function succeeded */
#define kFlashFailed 0x01 /* called function failed */
/* controller specific error codes - only an example!!! */
#define kFlashInvalidParam 0x02
#define kFlashInvalidAddress 0x03
#define kFlashInvalidSize 0x04
#define kFlashInvalidData 0x05
#define kFlashNoProgClock 0x06
#define kFlashNoProgVoltage 0x07
#define kFlashPViol 0x08
#define kFlashAccerr 0x09

#define FALSE 0

typedef unsigned char tMajorVersion; /* flash driver major version number */
typedef unsigned char tMinorVersion; /* flash driver minor version number */
typedef unsigned char tBugfixVersion; /* bugfix / patchlevel number */
typedef unsigned short tFlashResult; /* result of flash driver routines */
typedef unsigned long tAddress; /* logical address */
typedef unsigned long tLength; /* length (in bytes) */
typedef unsigned char tData; /* ambiguous data */
typedef void (*tWDTriggerFct) (void); /* watchdog trigger routine */

typedef struct
{
	/* initialization: inout parameters */
	tBugfixVersion patchlevel; /* flash driver patch level version */
	tMinorVersion minornumber; /* flash driver minor version number */
	tMajorVersion majornumber; /* flash driver major version number */
	unsigned char reserved1; /* reserved for future use, set to 0x00 for now */
	/* retrun value / error code: output parameters */
	tFlashResult errorcode; /* return value / error code */
	unsigned short reserved2; /* reserved for future use, set to 0x0000 for now */
	/* erase / write: input parameters */
	tAddress address; /* logical target address */
	tLength length; /* lenght information (in bytes) */
	tData *data; /* pointer to data buffer */
	/* additional input parameters */
	tWDTriggerFct wdTriggerFct; /* pointer to watchdog trigger routine */
	/* erase / write: optional output parameters: debugging information */
	tData intendedData[2]; /* intented data at error address */
	tData actualData[2]; /* actual data at error address */
	tAddress errorAddress; /* address of error */
	/* additonal controller / hardware specific parameters */
	/* --> to be specified by flash driver vendor */
}tFlashParam;

typedef void (*tFlashFct) (tFlashParam*); /* prototype of flash driver routine */


extern void DFlashInit(tFlashParam* FlashParam); /* initialization routine */
extern void DFlashDeinit(tFlashParam* FlashParam); /* de-initialization */
extern void DFlashErase(tFlashParam* FlashParam); /* erase / verify erase */
extern void DFlashWrite(tFlashParam* FlashParam); /* program / verify write */
extern void DFlashRead(tFlashParam* FlashParam);


#endif
