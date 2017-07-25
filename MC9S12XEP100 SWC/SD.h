#include <hidef.h>      /* common defines and macros */


#define CD PTIJ_PTIJ0
#define CD_dir DDRJ_DDRJ0
#define WP PTIJ_PTIJ1
#define WP_dir DDRJ_DDRJ1

#define DDR_INI()       DDRS |= 0xe0          
#define SD_select()        PTS_PTS7=0        //低电平
#define SD_deselect()      PTS_PTS7=1        //高电平
#define PTS_INIT()       DDRS |= 0xe0  


void SPI_Init(void); 
void SPI_4M(void); 
void clear_buffer(byte buffer[]);
byte SPI_Byte(byte value);
void SD_Init(void);
byte SD_send_command(byte cmd, long arg);
byte SD_Reset(void);
byte read_block(long sector, byte* buffer);
byte write_block(long sector, byte* buffer);
 
































