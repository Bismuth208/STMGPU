/*
* (c) Domen Puncer, Visionect, d.o.o.
* BSD License
*
* v0.2 add support for SDHC
*/

#include <stdio.h>
#include "diskio.h"
#include "sdcard_spi.h"
#include "integer.h"


//===========================================================================//


/*** fatfs code that uses the public API ***/


DSTATUS disk_initialize(BYTE drv)
{
  if (hwif_init(&hw) == 0)
    return 0;
  
  return STA_NOINIT;
}


DSTATUS disk_status(BYTE drv)
{
  if (hw.initialized)
    return 0;
  
  return STA_NOINIT;
}


DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
  int i;
  
  for (i=0; i<count; i++)
    if (sd_read(&hw, sector+i, buff+512*i) != 0)
      return RES_ERROR;
  
  return RES_OK;
}


#if _READONLY == 0
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
  int i;
  
  for (i=0; i<count; i++)
    if (sd_write(&hw, sector+i, buff+512*i) != 0)
      return RES_ERROR;
  
  return RES_OK;
}
#endif /* _READONLY */



DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
  switch (cmd) {
  case CTRL_SYNC:
    return RES_OK;
  case GET_SECTOR_SIZE:
    *(WORD*)buff = 512;
    return RES_OK;
  case GET_SECTOR_COUNT:
    *(DWORD*)buff = hw.sectors;
    return RES_OK;
  case GET_BLOCK_SIZE:
    *(DWORD*)buff = hw.erase_sectors;
    return RES_OK;
  }
  return RES_PARERR;
}

// FAT time structure discription
typedef struct
{
  DWORD sec: 	5;	// bits 0-4, 1 unit = 2 sec
  DWORD	min:	6;	// bits 5-10
  DWORD	hour:	5;	// bits 11-15
  DWORD	day:	5;	// bits 16-20
  DWORD	month:	4;	// bits 21-24
  DWORD	year:	7;	// bits 25-31
} fat_time_t;

DWORD get_fattime(void)
{
  union
  {
    fat_time_t fat_time;
    DWORD dword;
  } time;
  
  time.fat_time.sec = 0;
  time.fat_time.min = 0;
  time.fat_time.hour = 12;
  time.fat_time.day = 4;
  time.fat_time.month = 9;
  time.fat_time.year = 2013-1980;
  
  return time.dword;
  
}

WCHAR ff_convert (	/* Converted character, Returns zero on error */
                  WCHAR	src,	/* Character code to be converted */
                  UINT	dir		/* 0: Unicode to OEMCP, 1: OEMCP to Unicode */
                    )
{
  return src;
}


WCHAR ff_wtoupper (	/* Upper converted character */
                   WCHAR chr		/* Input character */
                     )
{
  return chr;
}
