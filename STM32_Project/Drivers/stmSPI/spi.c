#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <systicktimer.h>

#include <stm32f10x.h>

#include "spi.h"

//---------------------------------------------------------------------------------------------//

#define CR1_CLEAR_Mask       ((uint16_t)0x3040)
#define SPI_Mode_Select      ((uint16_t)0xF7FF)
#define CR1_SPE_Set          ((uint16_t)0x0040)

//---------------------------------------------------------------------------------------------//

void init_GPIO_SPI1(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //disable JTAG, SW left enabled
  //GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
  
  //Заполняем поля структуры нашими параметрами
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;      // Alternate fun mode
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SPI_LCD_MOSI | GPIO_Pin_SPI_LCD_SCK ;
  GPIO_Init(GPIO_SPI_LCD, &GPIO_InitStruct);        // pply settings
}

//--------------------------------------- SPI_1 ------------------------------------------------//
void init_SPI1(void)
{
  // Enable clock for SPI1
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);
  
  init_GPIO_SPI1();
  
  uint16_t tmpreg = (uint16_t)((uint32_t)SPI_Direction_1Line_Tx |       // .SPI_Direction
                        SPI_Mode_Master |                       // .SPI_Mode
                        SPI_DataSize_8b |                       // .SPI_DataSize
                        SPI_CPOL_Low |                          // .SPI_CPOL
                        SPI_CPHA_1Edge |                        // .SPI_CPHA
                        SPI_NSS_Soft |                          // .SPI_NSS
                        SPI_BaudRatePrescaler_2 |               // .SPI_BaudRatePrescaler
                        SPI_FirstBit_MSB |                      // .SPI_FirstBit
                        CR1_SPE_Set |                           // Enable SPI
                        SPI_NSSInternalSoft_Set  );             // 
  /*
  * Only Tx
  * Master mode
  * transfer size 8 bit
  * Polarity
  * Clock phase of signal
  * NSS by controled by soft
  * SCK devider = 2
  * First bit sended MSB
  */
  
  /* Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits */
  /* Write to SPIx CR1 */
  MODIFY_REG(SPI1->CR1, CR1_CLEAR_Mask, tmpreg);
  
  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPI1->I2SCFGR &= SPI_Mode_Select;
}

inline void sendData8_SPI1(uint8_t data)
{
  SPI1->DR = data;
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  //WAIT_FOR_END;
}

inline void sendData16_SPI1(uint16_t data)
{
  // this faster, than change data size...
  SPI1->DR = (data>>8);
  
  WAIT_FREE_TX;
  SPI1->DR = (data);
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  //WAIT_FOR_END;
}

inline void sendData32_SPI1(uint16_t data0, uint16_t data1)
{
  SET_BIT(SPI1->CR1, SPI_DataSize_16b);
  SPI1->DR = data0;
  
  WAIT_FREE_TX;
  SPI1->DR = data1;

  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  //WAIT_FOR_END;
  CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);
}

void sendArr8_SPI1(void *data, uint32_t size)
{
  for(uint32_t count =0; count < size; count++) {
    WAIT_FREE_TX;
    SPI1->DR = ((uint8_t*)data)[count];
  }
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
}

void sendArr16_SPI1(void *data, uint32_t size)
{
  SET_BIT(SPI1->CR1, SPI_DataSize_16b);
  
  for(uint32_t count =0; count < size; count++) {
    WAIT_FREE_TX;
    SPI1->DR = ((uint16_t*)data)[count];
  }
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);       // set 8bit size
}

void repeatData16_SPI1(uint16_t data, uint32_t size)
{
  SET_BIT(SPI1->CR1, SPI_DataSize_16b);
  
  while(size) {
    WAIT_FREE_TX;
    SPI1->DR = data;
    --size;
  }
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);       // set 8bit size
}
