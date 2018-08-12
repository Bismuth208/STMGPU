/*-----------------------------------------------------------------------
/  Low level disk interface module include file   (C)ChaN, 2013
/-----------------------------------------------------------------------*/

#ifndef _SDCARD_SPI_H
#define _SDCARD_SPI_H

#ifdef STM32F40XX
#include <stm32f4xx_conf.h>
#else
#include <stm32f10x_conf.h>
#endif /* STM32F40XX */

#include "integer.h"
  
/* SD card is connected to SPI1, PA4-7 */
#if defined(STM32F10X_MD) || defined(STM32F10X_HD)
#define CARD_SUPPLY_SWITCHABLE   0
#define SOCKET_WP_CONNECTED      0 /* write-protect socket-switch */
#define SOCKET_CP_CONNECTED      0 /* card-present socket-switch */
#define GPIO_WP                  GPIOC
#define GPIO_CP                  GPIOC
#define RCC_APBxPeriph_GPIO_WP   RCC_APB2Periph_GPIOC
#define RCC_APBxPeriph_GPIO_CP   RCC_APB2Periph_GPIOC
#define GPIO_Pin_WP              GPIO_Pin_6
#define GPIO_Pin_CP              GPIO_Pin_7
#define GPIO_Mode_WP             GPIO_Mode_IN_FLOATING /* external resistor */
#define GPIO_Mode_CP             GPIO_Mode_IN_FLOATING /* external resistor */
#define SPI_SD                   SPI2
#define GPIO_SPI_SD_CS           GPIOB
#define RCC_APB2Periph_GPIO_CS   RCC_APB2Periph_GPIOB
#define DMA_Channel_SPI_SD_RX    DMA1_Channel4
#define DMA_Channel_SPI_SD_TX    DMA1_Channel5
#define DMA_FLAG_SPI_SD_TC_RX    DMA1_FLAG_TC4
#define DMA_FLAG_SPI_SD_TC_TX    DMA1_FLAG_TC5
#define GPIO_SPI_SD              GPIOB
#define GPIO_Pin_SPI_SD_CS       GPIO_Pin_12
#define GPIO_Pin_SPI_SD_SCK      GPIO_Pin_13
#define GPIO_Pin_SPI_SD_MISO     GPIO_Pin_14
#define GPIO_Pin_SPI_SD_MOSI     GPIO_Pin_15
#define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB1PeriphClockCmd
#define RCC_APBPeriph_SPI_SD     RCC_APB1Periph_SPI2
#define SPI_BaudRatePrescaler_SPI_SD  SPI_BaudRatePrescaler_2
#else
#define CARD_SUPPLY_SWITCHABLE   0
#define SOCKET_WP_CONNECTED      0 /* write-protect socket-switch */
#define SOCKET_CP_CONNECTED      0 /* card-present socket-switch */
//#define GPIO_WP                  GPIOC
//#define GPIO_CP                  GPIOC
//#define RCC_APBxPeriph_GPIO_WP   RCC_APB2Periph_GPIOC
//#define RCC_APBxPeriph_GPIO_CP   RCC_APB2Periph_GPIOC
//#define GPIO_Pin_WP              GPIO_Pin_6
//#define GPIO_Pin_CP              GPIO_Pin_7
//#define GPIO_Mode_WP             GPIO_Mode_IN_FLOATING /* external resistor */
//#define GPIO_Mode_CP             GPIO_Mode_IN_FLOATING /* external resistor */
#define SPI_SD                   SPI3
#define GPIO_SPI_SD_CS           GPIOC
#define RCC_APB2Periph_GPIO_CS   RCC_APB1Periph_GPIOC
//#define DMA_Channel_SPI_SD_RX    DMA1_Channel4
//#define DMA_Channel_SPI_SD_TX    DMA1_Channel5
//#define DMA_FLAG_SPI_SD_TC_RX    DMA1_FLAG_TC4
//#define DMA_FLAG_SPI_SD_TC_TX    DMA1_FLAG_TC5
#define GPIO_SPI_SD              GPIOC
#define GPIO_Pin_SPI_SD_CS       GPIO_Pin_9
#define GPIO_Pin_SPI_SD_SCK      GPIO_Pin_10
#define GPIO_Pin_SPI_SD_MISO     GPIO_Pin_11
#define GPIO_Pin_SPI_SD_MOSI     GPIO_Pin_12
#define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB1PeriphClockCmd
#define RCC_APBPeriph_SPI_SD     RCC_APB1Periph_SPI3
#define SPI_BaudRatePrescaler_SPI_SD  SPI_BaudRatePrescaler_2
#endif


#define spi_cs_low() GPIO_SetBits(GPIO_SPI_SD_CS, GPIO_Pin_SPI_SD_CS);
#define spi_cs_high() GPIO_ResetBits(GPIO_SPI_SD_CS, GPIO_Pin_SPI_SD_CS);
    
#define WAIT_FOR_RX while((SPI_SD->SR & SPI_I2S_FLAG_RXNE) == 0)
  
  
#define CAP_VER2_00	(1<<0)
#define CAP_SDHC	(1<<1)
  
  
#define sd_get_r3 sd_get_r7

// set to 0 to remove CRC Check by CRC16CCITT standart
#define CALC_CRC16CCITT 0 


#define CMD0   0
#define CMD8   8
#define CMD9   9
#define CMD10  10
#define CMD13  13
#define CMD16  16
#define CMD17  17
#define CMD24  24
#define CMD55  55
#define CMD58  58
#define CMD59  59

#define ACMD41  41


#define READY_FOR_DATA 0x80
  
/*---------------------------------------*/
  
typedef struct  {
  int initialized;
  int sectors;
  int erase_sectors;
  int capabilities;
} hwif;


/*
// 5.2 CID register
#pragma pack(push, 1)
typedef struct {
  uint32_t manufacturerID :8;
  uint32_t OEMApplicationID :16;
  union {
    uint8_t productNameArr[5];
    uint32_t productName1   :32;
    uint32_t productName2   :8;
  };
  uint32_t productRevision :8;
  uint32_t productSerialNum :32;
  uint32_t reserved :4;
  uint32_t manufacturingDate  :12;
  uint32_t crc7     :7;
  uint32_t reserved :1;
} regCID_t;

typedef union {
  uint16_t data[3];
  struct {
    uint16_t S        :1;
    uint16_t D        :1;
    uint16_t Index    :6;
    
    uint16_t BSY      :1;
    uint16_t HCS      :1;
    uint16_t FB       :1;
    uint16_t XPC      :1;
    uint16_t Reserved :3;
    uint16_t S18R     :1;
    
    uint16_t OCR      :16;
    
    uint16_t Reserved :8;
    uint16_t CRC7     :7;
    uint16_t E        :1;
  };
} sdArg_t; // command structure for SD card ( ACMD41)
#pragma pack(pop)
*/

extern hwif hw;

enum sd_speed { SD_SPEED_INVALID, SD_SPEED_400KHZ, SD_SPEED_25MHZ };


/*---------------------------------------*/
void sd_spi_init(void);
void spi_set_speed(enum sd_speed speed);
u8 spi_txrx(u8 data);
u8 crc7_one(u8 t, u8 data);
u8 crc7(const u8 *p, int len);
u16 crc16_ccitt(u16 crc, u8 ser_data);
u16 crc16(const u8 *p, int len);
void sd_cmd(u8 cmd, u32 arg);
u8 sd_get_r1();
u16 sd_get_r2();
u8 sd_get_r7(u32 *r7);
void sd_nec();
int sd_init(hwif *hw);
int sd_read_status(hwif *hw);
int sd_get_data(hwif *hw, u8 *buf, int len);
int sd_put_data(hwif *hw, const u8 *buf, int len);
int sd_read_csd(hwif *hw);
int sd_read_cid(hwif *hw);
int sd_readsector(hwif *hw, u32 address, u8 *buf);
int sd_writesector(hwif *hw, u32 address, const u8 *buf);


int hwif_init(hwif* hw);
int sd_read(hwif* hw, u32 address, u8 *buf);
int sd_write(hwif* hw, u32 address,const u8 *buf);


#endif /* _SDCARD_SPI_H */
