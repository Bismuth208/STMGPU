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

/*
* Code is split into 2 parts:
* - generic SPI code: adapt for your MCU
* - sd card code, with crc7 and crc16 calculations
*   there's lots of it, but it's simple
*/


//===========================================================================//

SPI_InitTypeDef  SPI_InitStructure;

hwif hw;


//---------------------------------------------------------------------------------------------//
// https://www.pololu.com/docs/0J1?section=5.f
// https://github.com/hazelnusse/crc7/blob/master/crc7.cc

#define _getCRC7value(crc, data)  (CRCTable[(crc << 1) ^ data])

#define USE_PRECALC_CRC_SD

#ifndef USE_PRECALC_CRC_SD
uint8_t CRCPoly = 0x89;  // the value of our CRC-7 polynomial
uint8_t CRCTable[256];

void generateCRCTable(void)
{
  int i, j;
  
  // generate a table value for all 256 possible byte values
  for(i = 0; i < 256; i++) {
    CRCTable[i] = (i & 0x80) ? i ^ CRCPoly : i;
    for(j = 1; j < 8; j++) {
      CRCTable[i] <<= 1;
      if(CRCTable[i] & 0x80)
        CRCTable[i] ^= CRCPoly;
    }
  }
}
#else
const uint8_t CRCTable[256] = {
    0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
    0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
    0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
    0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
    0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
    0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
    0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
    0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
    0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
    0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
    0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
    0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
    0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
    0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
    0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
    0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
    0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
    0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
    0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
    0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
    0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
    0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
    0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
    0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
    0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
    0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
    0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
    0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
    0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
    0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
    0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
    0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};
#endif

// adds a message byte to the current CRC-7 to get a the new CRC-7
uint8_t CRCAdd(uint8_t crc, uint8_t message_byte)
{
  return CRCTable[(crc << 1) ^ message_byte];
}

// returns the CRC-7 for a message of "length" bytes
uint8_t getCRC(uint8_t *message, uint32_t length)
{
  uint8_t crc = 0;
  
  for(uint32_t i = 0; i < length; i++) {
    //crc = CRCAdd(crc, message[i]);
    crc = CRCTable[(crc << 1) ^ message[i]];
  } 
  
  return crc;
}
//---------------------------------------------------------------------------------------------//

//===========================================================================//

void vInit_SPI2(void)
{
  /* SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_SPI_SD; // 72000kHz/256=281kHz < 400kHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  
  SPI_Init(SPI_SD, &SPI_InitStructure);
  SPI_CalculateCRC(SPI_SD, DISABLE);
  SPI_Cmd(SPI_SD, ENABLE);
  
//#pragma diag_suppress=Pe550 // disable unused warning
  uint32_t dummyread;
  /* drain SPI */
  while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE) == RESET) { ; }
  dummyread = SPI_I2S_ReceiveData(SPI_SD);
//#pragma diag_default=Pe550  // restore unused warning
  
  spi_set_speed(SD_SPEED_400KHZ);
  
#ifndef USE_PRECALC_CRC_SD
  generateCRCTable();
#endif
}

void spi_set_speed(enum sd_speed speed)
{
  if (speed == SD_SPEED_400KHZ) {
    //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    CLEAR_BIT(SPI_SD->CR1, SPI_BaudRatePrescaler_2);
    SET_BIT(SPI_SD->CR1, SPI_BaudRatePrescaler_128);
    
  } else if (speed == SD_SPEED_25MHZ) {
    //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    CLEAR_BIT(SPI_SD->CR1, SPI_BaudRatePrescaler_128);
    SET_BIT(SPI_SD->CR1, SPI_BaudRatePrescaler_2);
  }
  // ^ with /2 APB1 this will be 15mhz/234k at 60mhz
  // 18/281 at 72. which is ok, 100<x<400khz, and <25mhz
  /*
  SPI_Cmd(SPI_SD, DISABLE);
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI_SD, &SPI_InitStructure);
  SPI_Cmd(SPI_SD, ENABLE);
  */
}

u8 spi_txrx(u8 data)
{
  /* RXNE always happens after TXE, so if this function is used
  * we don't need to check for TXE */
  SPI_SD->DR = data;
  WAIT_FOR_RX;
  
  return SPI_SD->DR;
}

u16 spi_txrx16(u16 data)
{
  u16 r;
  
  SPI_SD->DR = data>>8;
  WAIT_FOR_RX;
  r = SPI_SD->DR <<8;
  
  SPI_SD->DR = data;
  WAIT_FOR_RX;
  r |= SPI_SD->DR;
  
  return r;
}

u32 spi_txrx32(uint32_t data)
{
  union uResult_t {
    uint8_t resultArr[4];
    uint32_t result32;
  } uResult;
  
  SPI_SD->DR = (data>>24);
  WAIT_FOR_RX;
  uResult.resultArr[3] = SPI_SD->DR;
  
  SPI_SD->DR = (data>>16);
  WAIT_FOR_RX;
  uResult.resultArr[2] = SPI_SD->DR;
  
  SPI_SD->DR = (data>>8);
  WAIT_FOR_RX;
  uResult.resultArr[1] = SPI_SD->DR;
  
  SPI_SD->DR = (data);
  WAIT_FOR_RX;
  uResult.resultArr[0] = SPI_SD->DR;
  
  return uResult.result32;
}

void spi_txrxArr(u8 *buf, u16 len, u8 data)
{
  while(len--) {
    SPI_SD->DR = data;
    WAIT_FOR_RX;
    
    *buf++ = SPI_SD->DR;
  }
}

/* crc helpers */
u8 crc7_one(u8 t, u8 data)
{
  int i;
  const u8 g = 0x89;
  
  t ^= data;
  for (i=0; i<8; i++) {
    if (t & 0x80)
      t ^= g;
    t <<= 1;
  }
  return t;
}

u8 crc7(const u8 *p, int len)
{
  int j;
  u8 crc = 0;
  for (j=0; j<len; j++)
    crc = crc7_one(crc, p[j]);
  
  return crc>>1;
}

/* http://www.eagleairaust.com.au/code/crc16.htm */
u16 crc16_ccitt(u16 crc, u8 ser_data)
{
  crc  = (u8)(crc >> 8) | (crc << 8);
  crc ^= ser_data;
  crc ^= (u8)(crc & 0xff) >> 4;
  crc ^= (crc << 8) << 4;
  crc ^= ((crc & 0xff) << 4) << 1;
  
  return crc;
}

u16 crc16(const u8 *p, int len)
{
  u16 crc = 0;
  while(len--) {
    crc = crc16_ccitt(crc, *p++);
  }
  return crc;
}


/*** sd functions - on top of spi code ***/
void sd_cmd(u8 cmd, u32 arg)
{
  u8 crc = CRCAdd(0, 0x40 | cmd);
  crc = CRCAdd(crc, arg >> 24);
  crc = CRCAdd(crc, arg >> 16);
  crc = CRCAdd(crc, arg >> 8);
  crc = CRCAdd(crc, arg);
  crc += crc;
  
  spi_txrx(0x40 | cmd);
  spi_txrx32(arg);
  spi_txrx(crc | 0x1);	/* crc7 */
}

void sd_cmd0(void)
{
  spi_txrx(0x40);
  spi_txrx32(0x00000000);
  spi_txrx(0x95);	/* crc7, for cmd0 */
}

u8 sd_get_r1()
{
  int tries = 1000;
  u8 r;
  
  while (tries--) {
    r = spi_txrx(0xff);
    if ((r & READY_FOR_DATA) == 0)
      return r;
  }
  return 0xff;
}

u16 sd_get_r2()
{
  int tries = 1000;
  u16 r;
  
  while (tries--) {
    r = spi_txrx(0xff);
    if ((r & READY_FOR_DATA) == 0)
      break;
  }
  if (tries < 0)
    return 0xff;
  r = r<<8 | spi_txrx(0xff);
  
  return r;
}

/*
* r1, then 32-bit reply... same format as r3
*/
u8 sd_get_r7(u32 *r7)
{
  u32 r;
  r = sd_get_r1();
  if (r != 0x01)
    return r;
  
  *r7 = spi_txrx32(0xffffffff);
  return 0x01;
}


/* Nec (=Ncr? which is limited to [0,8]) dummy bytes before lowering CS,
* as described in sandisk doc, 5.4. */
void sd_nec()
{
  spi_txrx32(0xffffffff);
  spi_txrx32(0xffffffff);
}


int sd_init(hwif *hw)
{
  int i;
  int r;
  u32 r7;
  u32 r3;
  int tries;
  
  hw->capabilities = 0;
  
  /* start with 100-400 kHz clock */
  spi_set_speed(SD_SPEED_400KHZ);
  
  /* cmd0 - reset.. */
  spi_cs_high();
  /* 74+ clocks with CS high */
  for (i=0; i<5; i++)
    spi_txrx16(0xffff);
  
  /* reset */
  spi_cs_low();
  sd_cmd0();
  r = sd_get_r1();
  //sd_nec();
  spi_cs_high();
  
  if (r == 0xff)
    return -1;
  if (r != 0x01) {
    /* fail */
    return -2;
  }
  /* success */
  
  /* cmd8 - voltage.. */
  /* ask about voltage supply */
  spi_cs_low();
  sd_cmd(CMD8, 0x1aa /* VHS = 1 */);
  r = sd_get_r7(&r7);
  sd_nec();
  spi_cs_high();
  hw->capabilities |= CAP_VER2_00;
  
  if (r == 0xff)
    return -1;
  if (r == 0x01);
  /* success, SD v2.x */
  else if (r & 0x4) {
    hw->capabilities &= ~CAP_VER2_00;
    /* not implemented, SD v1.x */
  } else {
    /* fail */
    return -2;
  } 
  
  /* cmd58 - ocr.. */
  /* ask about voltage supply */
  spi_cs_low();
  sd_cmd(CMD58, 0);
  r = sd_get_r3(&r3);
  sd_nec();
  spi_cs_high();
  
  if (r == 0xff)
    return -1;
  if (r != 0x01 && !(r & 0x4)) { /* allow it to not be implemented - old cards */
    /* fail */
    return -2;
  }
  else {
    int i;
    for (i=4; i<=23; i++)
      if (r3 & 1<<i)
        break;
    for (i=23; i>=4; i--)
      if (r3 & 1<<i)
        break;
    /* CCS shouldn't be valid here yet */

    /* success */
  }
  
  
  /* acmd41 - hcs.. */
  tries = 1000;
  u32 hcs = 0;
  /* say we support SDHC */
  if (hw->capabilities & CAP_VER2_00)
    hcs = 1<<30;
  
  /* needs to be polled until in_idle_state becomes 0 */
  do {
    /* send we don't support SDHC */
    spi_cs_low();
    /* next cmd is ACMD */
    sd_cmd(CMD55, 0);
    r = sd_get_r1();
    sd_nec();
    spi_cs_high();
    if (r == 0xff)
      return -1;
    /* well... it's probably not idle here, but specs aren't clear */
    if (r & 0xfe) {
      /* fail */
      return -2;
    }
    
    spi_cs_low();
    sd_cmd(ACMD41, hcs);
    r = sd_get_r1();
    sd_nec();
    spi_cs_high();
    if (r == 0xff)
      return -1;
    if (r & 0xfe) {
      /* fail */
      return -2;
    }
  } while (r != 0 && tries--);
  if (tries == -1) {
    /* timeouted */
    return -2;
  }
  /* success */
  
  /* Seems after this card is initialized which means bit 0 of R1
  * will be cleared. Not too sure. */
  
  
  if (hw->capabilities & CAP_VER2_00) {
    /* cmd58 - ocr, 2nd time.. */
    /* ask about voltage supply */
    spi_cs_low();
    sd_cmd(CMD58, 0);
    r = sd_get_r3(&r3);
    sd_nec();
    spi_cs_high();
    if (r == 0xff)
      return -1;
    if (r & 0xfe) {
      /* fail */
      return -2;
    }
    else {
#if 1
      int i;
      for (i=4; i<=23; i++)
        if (r3 & 1<<i)
          break;
      for (i=23; i>=4; i--)
        if (r3 & 1<<i)
          break;
      /* CCS shouldn't be valid here yet */

      // XXX power up status should be 1 here, since we're finished initializing, but it's not. WHY?
      // that means CCS is invalid, so we'll set CAP_SDHC later
#endif
      if (r3>>30 & 1) {
        hw->capabilities |= CAP_SDHC;
      }
      
      /* success */
    }
  }
  
  /* with SDHC block length is fixed to 1024 */
  if ((hw->capabilities & CAP_SDHC) == 0) {
    /* cmd16 - block length.. */
    spi_cs_low();
    sd_cmd(CMD16, 512);
    r = sd_get_r1();
    sd_nec();
    spi_cs_high();
    if (r == 0xff)
      return -1;
    if (r & 0xfe) {
      /* fail */
      return -2;
    }
    /* success */
  } 
  
  /* cmd59 - enable crc.. */
  /* crc on */
  spi_cs_low();
  sd_cmd(CMD59, 0);
  r = sd_get_r1();
  sd_nec();
  spi_cs_high();
  if (r == 0xff)
    return -1;
  if (r & 0xfe) {
    /* fail */
    return -2;
  }
  /* success */
  
  /* now we can up the clock to <= 25 MHz */
  spi_set_speed(SD_SPEED_25MHZ);
  
  return 0;
}

int sd_read_status(hwif *hw)
{
  u16 r2;
  
  spi_cs_low();
  sd_cmd(CMD13, 0);
  r2 = sd_get_r2();
  //sd_nec();
  spi_cs_high();
  if (r2 & 0x8000)
    return -1;
  
  return 0;
}

/* 0xfe marks data start, then len bytes of data and crc16 */
int sd_get_data(hwif *hw, u8 *buf, int len)
{
  int tries = 20000;
  u8 r;
#if CALC_CRC16CCITT
  u16 _crc16;
  u16 calc_crc;
#endif
  while (tries--) {
    r = spi_txrx(0xff);
    if (r == 0xfe)
      break;
  }
  if (tries < 0)
    return -1;
  
  spi_txrxArr(buf, len, 0xff);
 
#if CALC_CRC16CCITT
  _crc16 = spi_txrx16(0xffff);

  calc_crc = crc16(buf, len);
  if (_crc16 != calc_crc) {
    return -1;
  }
#else
  (void)spi_txrx16(0xffff);
#endif
  
  return 0;
}

int sd_put_data(hwif *hw, const u8 *buf, int len)
{
  u8 r;
  int tries = 10;
  u16 crc;
  
  spi_txrx(0xfe); /* data start */
  
  while (len--)
    spi_txrx(*buf++);
  
  crc = crc16(buf, len);
  /* crc16 */
  spi_txrx(crc>>8);
  spi_txrx(crc);
  
  /* normally just one dummy read in between... specs don't say how many */
  while (tries--) {
    r = spi_txrx(0xff);
    if (r != 0xff)
      break;
  }
  if (tries < 0)
    return -1;
  
  /* poll busy, about 300 reads for 256 MB card */
  tries = 100000;
  while (tries--) {
    if (spi_txrx(0xff) == 0xff)
      break;
  }
  if (tries < 0)
    return -2;
  
  /* data accepted, WIN */
  if ((r & 0x1f) == 0x05)
    return 0;
  
  return r;
}

int sd_read_csd(hwif *hw)
{
  u8 buf[16];
  int r;
  int capacity;
  
  spi_cs_low();
  sd_cmd(CMD9, 0);
  r = sd_get_r1();
  if (r == 0xff) {
    spi_cs_high();
    return -1;
  }
  if (r & 0xfe) {
    spi_cs_high();
    return -2;
  }
  
  r = sd_get_data(hw, buf, 16);
  //sd_nec();
  spi_cs_high();
  if (r == -1) {
    /* failed to get csd */
    return -3;
  }
  
  if ((buf[0] >> 6) + 1 == 1) {
    /* CSD v1 */
    capacity = (((buf[6]&0x3)<<10 | buf[7]<<2 | buf[8]>>6)+1) << (2+(((buf[9]&3) << 1) | buf[10]>>7)) << ((buf[5] & 0xf) - 9);
    
  } else {
    /* CSD v2 */
    /* this means the card is HC */
    hw->capabilities |= CAP_SDHC;
    
    capacity = buf[7]<<16 | buf[8]<<8 | buf[9]; /* in 512 kB */
    capacity *= 1024; /* in 512 B sectors */
  }
  
  hw->sectors = capacity;
  
  /* if erase_blk_en = 0, then only this many sectors can be erased at once
  * this is NOT yet tested */
  hw->erase_sectors = 1;
  if (((buf[10]>>6)&1) == 0)
    hw->erase_sectors = ((buf[10]&0x3f)<<1 | buf[11]>>7) + 1;
  
  return 0;
}

int sd_read_cid(hwif *hw)
{
  u8 buf[16];
  int r;
  
  spi_cs_low();
  sd_cmd(CMD10, 0);
  r = sd_get_r1();
  if (r == 0xff) {
    spi_cs_high();
    return -1;
  }
  if (r & 0xfe) {
    spi_cs_high();
    return -2;
  }
  
  r = sd_get_data(hw, buf, 16);
  //sd_nec();
  spi_cs_high();
  if (r == -1) {
    /* failed to get cid */
    return -3;
  }
  
  return 0;
}


int sd_readsector(hwif *hw, u32 address, u8 *buf)
{
  int r;
  
  spi_cs_low();
  if (hw->capabilities & CAP_SDHC)
    sd_cmd(CMD17, address); /* read single block */
  else
    sd_cmd(CMD17, address*512); /* read single block */
  
  r = sd_get_r1();
  if ((r == 0xff) || (r & 0xfe)) {
    spi_cs_high();
    r = -1;
    return r;
  }
  
  r = sd_get_data(hw, buf, 512);
  //sd_nec();
  spi_cs_high();
  if (r == -1) {
    r = -3;
    return r;
  }
  
  return 0;
}

int sd_writesector(hwif *hw, u32 address, const u8 *buf)
{
  int r;
  
  spi_cs_low();
  if (hw->capabilities & CAP_SDHC)
    sd_cmd(CMD24, address); /* write block */
  else
    sd_cmd(CMD24, address*512); /* write block */
  
  r = sd_get_r1();
  if (r == 0xff) {
    spi_cs_high();
    r = -1;
    return r;
  }
  if (r & 0xfe) {
    spi_cs_high();
    r = -2;
    return r;
  }
  
  spi_txrx(0xff); /* Nwr (>= 1) high bytes */
  r = sd_put_data(hw, buf, 512);
  sd_nec();
  spi_cs_high();
  if (r != 0) {
    r = -3;
    return r;
  }
  
  /* efsl code is weird shit, 0 is error in there?
  * not that it's properly handled or anything,
  * and the return type is char, fucking efsl */
  return 0;
}


/*** public API - on top of sd/spi code ***/

int hwif_init(hwif* hw)
{
  int tries = 10;
  
  if (hw->initialized)
    return 0;
  
  while (tries--) {
    if (sd_init(hw) == 0)
      break;
  }
  if (tries == -1)
    return -1;
  
  /* read status register */
  sd_read_status(hw);
  
  sd_read_cid(hw);
  if (sd_read_csd(hw) != 0)
    return -1;
  
  hw->initialized = 1;
  return 0;
}

int sd_read(hwif* hw, u32 address, u8 *buf)
{
  int r;
  int tries = 10;
  
  r = sd_readsector(hw, address, buf);
  
  if(r < 0) {
    while (r < 0 && tries--) {
      if (sd_init(hw) != 0)
        continue;
      
      /* read status register */
      sd_read_status(hw);
      
      r = sd_readsector(hw, address, buf);
    }
  }
  
  //if (tries == -1)
  //printf("%s: couldn't read sector %li\n", __func__, address);
  
  return r;
}

int sd_write(hwif* hw, u32 address,const u8 *buf)
{
  int r;
  int tries = 10;
  
  r = sd_writesector(hw, address, buf);
  
  while (r < 0 && tries--) {
    if (sd_init(hw) != 0)
      continue;
    
    /* read status register */
    sd_read_status(hw);
    
    r = sd_writesector(hw, address, buf);
  }
  //if (tries == -1)
  //printf("%s: couldn't write sector %li\n", __func__, address);
  
  return r;
}
